#pragma once

#include "stdafx.h"

#include "Actor.h"
#include "CommandArgs.h"

class InputHandler;

namespace game_scene {
namespace actors {

class IOInterfaceImpl : public ActorImpl {
public:
	IOInterfaceImpl(InputHandler& input_handler);

	void HandleCommand(const CommandArgs& args);
	void AddedToScene();

	enum class ListenerId : unsigned char {
		MOUSE_MOTION = 0,
		KEY_PRESS,
		KEY_RELEASE,
		KEY_TOGGLE,
		NUM_LISTENERS,
	};

private:
	// Functions that control which keys are watched to determine if they are pressed or released.
	void WatchKey(unsigned char key_to_watch);
	// Possibly later add the ability to unwatch keys. Currently once they're watched they're always watched.
	
	// State for what keys are watched. Only check the key's state and alert listeners if something actually cares.
	array<unsigned char, 256> which_keys_to_watch_;
	uint8_t num_keys_watched_;
	array<bool, 256> is_key_watched_;
	
	// Listener groups that are alerted when a relevant thing happens.
	ActorId mouse_motion_listener_group_;
	array<ActorId, 256> key_press_listener_groups_;
	array<ActorId, 256> key_release_listener_groups_;
	array<ActorId, 256> key_toggle_listener_groups_;
	ActorId& GetKeyActionListenerGroup(ListenerId listener_type, unsigned char key);

	InputHandler& input_handler_;
};
ADD_ACTOR_ADAPTER(IOInterface);

}  // actors

class IOInterfaceCommand {
public:
	DECLARE_COMMAND(IOInterfaceCommand, REGISTER_LISTENER);
	DECLARE_COMMAND(IOInterfaceCommand, LISTEN_MOUSE_MOTION);
	DECLARE_COMMAND(IOInterfaceCommand, LISTEN_KEY_PRESS);
	DECLARE_COMMAND(IOInterfaceCommand, LISTEN_KEY_RELEASE);
	DECLARE_COMMAND(IOInterfaceCommand, LISTEN_KEY_TOGGLE);
};

namespace commands {
class IOListenerRegistration : public CommandArgs {
public:
	IOListenerRegistration(
		bool register_not_unregister,
		ActorId actor_to_register,
		actors::IOInterface::ListenerId listener_id) :
		CommandArgs(IOInterfaceCommand::REGISTER_LISTENER),
		register_not_unregister_(register_not_unregister),
		actor_to_register_(actor_to_register),
		listener_id_(listener_id) {}

	IOListenerRegistration(
		bool register_not_unregister,
		ActorId actor_to_register,
		actors::IOInterface::ListenerId listener_id,
		unsigned char key) :
		CommandArgs(IOInterfaceCommand::REGISTER_LISTENER),
		register_not_unregister_(register_not_unregister),
		actor_to_register_(actor_to_register),
		listener_id_(listener_id) {
		keys_.push_back(key);
	}

	IOListenerRegistration(
		bool register_not_unregister,
		ActorId actor_to_register,
		actors::IOInterface::ListenerId listener_id,
		vector<unsigned char> keys) :
		CommandArgs(IOInterfaceCommand::REGISTER_LISTENER),
		register_not_unregister_(register_not_unregister),
		actor_to_register_(actor_to_register),
		listener_id_(listener_id), keys_(move(keys)) {}

	bool register_not_unregister_;
	ActorId actor_to_register_;
	actors::IOInterface::ListenerId listener_id_;
	// Which key to register for. Is unused for non-key related registrations.
	vector<unsigned char> keys_;
};

class MouseMotion : public CommandArgs {
public:
	MouseMotion(array<int, 2> motion) :
		CommandArgs(IOInterfaceCommand::LISTEN_MOUSE_MOTION), motion_(motion) {}

	array<int, 2> motion_;
};

class KeyPress : public CommandArgs {
public:
	KeyPress(unsigned char key) :
		CommandArgs(IOInterfaceCommand::LISTEN_KEY_PRESS), key_(key) {}

	unsigned char key_;
};

class KeyRelease : public CommandArgs {
public:
	KeyRelease(unsigned char key) :
		CommandArgs(IOInterfaceCommand::LISTEN_KEY_RELEASE), key_(key) {}

	unsigned char key_;
};

class KeyToggle : public CommandArgs {
public:
	KeyToggle(unsigned char key, bool pressed) :
		CommandArgs(IOInterfaceCommand::LISTEN_KEY_TOGGLE),
		key_(key), pressed_(pressed) {}

	bool pressed_;
	unsigned char key_;
};

}  // commands
}  // game_scene