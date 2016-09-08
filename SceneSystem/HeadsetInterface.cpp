#include "stdafx.h"

#include "HeadsetInterface.h"
#include "InputCommandArgs.h"
#include "GraphicsObject.h"
#include "Scene.h"

namespace game_scene {

REGISTER_COMMAND(HeadsetInterfaceCommand, REGISTER_LISTENER);
REGISTER_COMMAND(HeadsetInterfaceCommand, LISTEN_TRIGGER_STATE_CHANGE);
REGISTER_COMMAND(HeadsetInterfaceCommand, LISTEN_TOUCHPAD_SLIDE);
REGISTER_COMMAND(HeadsetInterfaceCommand, LISTEN_TOUCHPAD_DRAG);
REGISTER_COMMAND(HeadsetInterfaceCommand, LISTEN_CONTROLLER_MOVEMENT);

namespace actors {

namespace HeadsetHelpers {
bool IsTriggerPulled(const vr::VRControllerState_t& state) {
	return state.rAxis[1].x > 0.1f;
}
}  // HeadsetHelpers

void HeadsetInterface::AddedToScene() {
	if (scene_->FindByName("HeadsetInterface") != ActorId::UnsetId) {
		std::cout << "Attempting to register a second HeadsetInterface instance" << std::endl;
		return;
	}
	scene_->RegisterByName("HeadsetInterface", id_);

	controller_graphics_[0] = CreateControllerActor();
	controller_graphics_[1] = CreateControllerActor();
}

void HeadsetInterface::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CommandType::ADDED_TO_SCENE:
	{
		for (ActorId& listener_group : listener_groups_) {
			listener_group = scene_->AddActorGroup();
		}
	}
	break;
	case HeadsetInterfaceCommand::REGISTER_LISTENER:
	{
		const commands::ListenerRegistration& registration = dynamic_cast<const commands::ListenerRegistration&>(args);
		if (registration.register_not_unregister_) {
			scene_->AddActorToGroup(registration.actor_to_register_, listener_groups_[static_cast<size_t>(registration.listener_id_)]);
		} else {
			scene_->RemoveActorFromGroup(registration.actor_to_register_, listener_groups_[static_cast<size_t>(registration.listener_id_)]);
		} 
	}
	break;
	case commands::InputCommandType::TICK:
		for (unsigned int i = 0; i < 2; i++) {
			vr::TrackedDeviceIndex_t controller_index = headset_.GetDeviceIndex(vr::TrackedDeviceClass_Controller, i);
			if (controller_index == vr::k_unTrackedDeviceIndexInvalid) {
				if (controller_connectedness_[i]) {
					// Should make the actor invisible here.
				}
				controller_connectedness_[i] = false;
				continue;
			}
			if (!controller_connectedness_[i]) {
				// Should make the actor visible here.
			}
			controller_connectedness_[i] = true;

			// Update the controller pose.
			Pose new_controller_position = headset_.GetGamePose(controller_index);
			scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
				Target(listener_groups_[static_cast<size_t>(ListenerId::CONTROLLER_MOVEMENT)]),
				make_unique<commands::ControllerMovement>(
					i, new_controller_position.location_ + (controller_positions_[i].location_ * -1))));
			controller_positions_[i] = new_controller_position;
			scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
				Target(controller_graphics_[i]),
				make_unique<commands::ComponentPlacement>("sphere", DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f)*controller_positions_[i].GetMatrix())));

			// Update the button state of the controller.
			HandleNewControllerState(i, headset_.GetGameControllerState(controller_index));
		}
	}
}

void HeadsetInterface::HandleNewControllerState(int hand_index, vr::VRControllerState_t new_state) {
	bool trigger_is_pulled = HeadsetHelpers::IsTriggerPulled(new_state);
	bool trigger_was_pulled = HeadsetHelpers::IsTriggerPulled(controller_states_[hand_index]);
	// If the trigger's state has changed, alert all listeners.
	if (trigger_is_pulled != trigger_was_pulled) {
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(listener_groups_[static_cast<size_t>(ListenerId::TRIGGER_STATE_CHANGE)]),
			make_unique<commands::TriggerStateChange>(
				hand_index, trigger_is_pulled)));
	}

	vr::VRControllerAxis_t touchpad_delta;
	touchpad_delta.x = new_state.rAxis[0].x - controller_states_[hand_index].rAxis[0].x;
	touchpad_delta.y = new_state.rAxis[0].y - controller_states_[hand_index].rAxis[0].y;

	if ((vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) & new_state.ulButtonTouched) &&
		(vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) & controller_states_[hand_index].ulButtonTouched)) {
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(listener_groups_[static_cast<size_t>(ListenerId::TOUCHPAD_SLIDE)]),
			make_unique<commands::TouchpadMotion>(HeadsetInterfaceCommand::LISTEN_TOUCHPAD_SLIDE, hand_index, new_state.rAxis[0], touchpad_delta)));
	}
	if ((vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) & new_state.ulButtonPressed) &&
		(vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) & controller_states_[hand_index].ulButtonPressed)) {
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(listener_groups_[static_cast<size_t>(ListenerId::TOUCHPAD_SLIDE)]),
			make_unique<commands::TouchpadMotion>(HeadsetInterfaceCommand::LISTEN_TOUCHPAD_DRAG, hand_index, new_state.rAxis[0], touchpad_delta)));
	}
	controller_states_[hand_index] = new_state;
}

ActorId HeadsetInterface::CreateControllerActor() {
	game_scene::actors::GraphicsObjectDetails sphere_details;
	sphere_details.heirarchy_ = game_scene::actors::ComponentHeirarchy(
		"sphere",
		{
			{"sphere.obj|Sphere",
			ObjLoader::OutputFormat(
				ModelModifier(
					{0, 1, 2},
					{1, 1, 1},
					{false, true}),
				ObjLoader::vertex_type_all,
				false)}
		},
		{});
	sphere_details.heirarchy_.shader_file_definition_ = game_scene::actors::ShaderFileDefinition("solidcolor.hlsl");
	sphere_details.heirarchy_.vertex_shader_input_type_ = ObjLoader::vertex_type_texture;
	sphere_details.heirarchy_.entity_group_ = "basic";
	sphere_details.heirarchy_.shader_settings_ = {
		{0.0f, 0.0f, 0.5f},
	};

	ActorId new_controller = scene_->AddActor(make_unique<game_scene::actors::GraphicsObject>());
	CommandQueueLocation created_actor =
	scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(new_controller), 
		make_unique<game_scene::WrappedCommandArgs<game_scene::actors::GraphicsObjectDetails>>(
			game_scene::GraphicsObjectCommand::CREATE_COMPONENTS,
			sphere_details)));
	scene_->MakeCommandAfter(created_actor, Command(
		game_scene::Target(new_controller),
		make_unique<game_scene::commands::ComponentPlacement>("sphere", DirectX::XMMatrixScaling(0.1f, 0.1f, 0.1f))));
	return new_controller;
}

}  // actors
}  // game_scene
