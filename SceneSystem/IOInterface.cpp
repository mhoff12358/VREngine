#include "stdafx.h"
#include "IOInterface.h"

#include "InputCommandArgs.h"
#include "Scene.h"

namespace game_scene {
namespace actors {

void IOInterface::AddedToScene() {
	mouse_motion_listener_group_ = scene_->AddActorGroup();
}

IOInterface::IOInterface(InputHandler& input_handler) : input_handler_(input_handler) {
	// Set all the listener groups to the unset id.
	mouse_motion_listener_group_ = ActorId::UnsetId;
	for (ActorId& listener_group : key_press_listener_groups_) {
		listener_group = ActorId::UnsetId;
	}
	for (ActorId& listener_group : key_release_listener_groups_) {
		listener_group = ActorId::UnsetId;
	}
	for (ActorId& listener_group : key_toggle_listener_groups_) {
		listener_group = ActorId::UnsetId;
	}
	// Zero out all the keys watched values.
	num_keys_watched_ = 0;
	for (int i = 0; i < 256; i++) {
		is_key_watched_[i] = false;
	}
}

void IOInterface::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case IOInterfaceCommand::REGISTER_LISTENER:
	{
		const commands::IOListenerRegistration& registration = dynamic_cast<const commands::IOListenerRegistration&>(args);
		ActorId listener_group_to_register;  // The listener group to either register or unregister from.
		// Set the listener group to register based on the new group.
		if (registration.listener_id_ == ListenerId::MOUSE_MOTION) {
			listener_group_to_register = mouse_motion_listener_group_;
			if (registration.register_not_unregister_) {
				scene_->AddActorToGroup(registration.actor_to_register_, listener_group_to_register);
			}
			else {
				scene_->RemoveActorFromGroup(registration.actor_to_register_, listener_group_to_register);
			}
		} else {
			// The registration is for a specific key's action.
			// The listener id specifies which set of listeners to search through.
			// The key specifies which specific listener to use.

			// Lookup the actual registration group's existing ID.
			for (unsigned char key : registration.keys_) {
				ActorId& listener_group_ref = GetKeyActionListenerGroup(registration.listener_id_, key);
				// If we are registering and the registration group doesn't exist then create it and watch the key.
				if (registration.register_not_unregister_ && listener_group_ref == ActorId::UnsetId) {
					listener_group_ref = scene_->AddActorGroup();
					WatchKey(key);
				}
				listener_group_to_register = listener_group_ref;
				if (registration.register_not_unregister_) {
					scene_->AddActorToGroup(registration.actor_to_register_, listener_group_to_register);
				}
				else {
					scene_->RemoveActorFromGroup(registration.actor_to_register_, listener_group_to_register);
				}
			}
		}
	}
		break;
	case InputCommand::TICK:
		// Updates the keyboard state since the last tick.
		input_handler_.UpdateKeyboardState();
		// Consumes any mouse motions since the last tick and passes them on to all listeners.
		scene_->MakeCommandAfter(scene_->FrontOfCommands(),
			Command(
				Target(mouse_motion_listener_group_),
				make_unique<commands::MouseMotion>(input_handler_.ConsumeMouseMotion())));
		// Checks the watched keys.
		for (unsigned int key_index = 0; key_index < num_keys_watched_; key_index++) {
			unsigned char key = which_keys_to_watch_[key_index];
			bool pressed = input_handler_.GetKeyToggled(key);
			bool released = input_handler_.GetKeyToggled(key, false);
			if (pressed) {
				scene_->MakeCommandAfter(scene_->FrontOfCommands(),
					Command(
						Target(key_press_listener_groups_[key]),
						make_unique<commands::KeyPress>(key)));
			} else if (released) {
				scene_->MakeCommandAfter(scene_->FrontOfCommands(),
					Command(
						Target(key_release_listener_groups_[key]),
						make_unique<commands::KeyRelease>(key)));
			}
			if (pressed || released) {
				scene_->MakeCommandAfter(scene_->FrontOfCommands(),
					Command(
						Target(key_toggle_listener_groups_[key]),
						make_unique<commands::KeyToggle>(key, pressed)));
			}
		}
		break;
	}
}

void IOInterface::WatchKey(unsigned char key_to_watch) {
	if (!is_key_watched_[key_to_watch]) {
		is_key_watched_[key_to_watch] = true;
		which_keys_to_watch_[num_keys_watched_++] = key_to_watch;
	}
}

ActorId& IOInterface::GetKeyActionListenerGroup(ListenerId listener_type, unsigned char key) {
	switch (listener_type) {
	case ListenerId::KEY_PRESS:
		return key_press_listener_groups_[key];
	case ListenerId::KEY_RELEASE:
		return key_release_listener_groups_[key];
	case ListenerId::KEY_TOGGLE:
		return key_toggle_listener_groups_[key];
	default:
		std::cout << "IOInterface looking up a key listener group, but with mouse motion listener" << std::endl;
		return mouse_motion_listener_group_;
	}
}

}  // actors
}  // game_scene