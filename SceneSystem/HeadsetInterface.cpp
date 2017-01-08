#include "stdafx.h"

#include "HeadsetInterface.h"
#include "InputCommandArgs.h"
#include "GraphicsObject.h"
#include "Scene.h"
#include "VRBackend/Headset.h"

constexpr float controller_sphere_radius = 0.025f;

namespace game_scene {

REGISTER_COMMAND(HeadsetInterfaceCommand, REGISTER_LISTENER);
REGISTER_COMMAND(HeadsetInterfaceCommand, LISTEN_TRIGGER_STATE_CHANGE);
REGISTER_COMMAND(HeadsetInterfaceCommand, LISTEN_TOUCHPAD_SLIDE);
REGISTER_COMMAND(HeadsetInterfaceCommand, LISTEN_TOUCHPAD_DRAG);
REGISTER_COMMAND(HeadsetInterfaceCommand, LISTEN_CONTROLLER_MOVEMENT);
REGISTER_COMMAND(HeadsetInterfaceCommand, HAPTIC_PULSE);

namespace actors {

namespace HeadsetHelpers {
bool IsTriggerPulled(const vr::VRControllerState_t& state) {
	return state.rAxis[1].x > 0.1f;
}
}  // HeadsetHelpers

void HeadsetInterface::AddedToScene() {
	controller_graphics_[0] = CreateControllerActor();
	controller_graphics_[1] = CreateControllerActor();
	
	for (auto& controller_listener_groups : listener_groups_) {
		for (ActorId& listener_group : controller_listener_groups) {
			listener_group = scene_->AddActorGroup();
		}
	}
}

void HeadsetInterface::RegisterOrUnregisterActor(bool register_not_unregister, ActorId actor, unsigned char controller_index, ListenerId listener_id) {
	if (register_not_unregister) {
		scene_->AddActorToGroup(actor, listener_groups_[controller_index][static_cast<size_t>(listener_id)]);
	} else {
		scene_->RemoveActorFromGroup(actor, listener_groups_[controller_index][static_cast<size_t>(listener_id)]);
	} 
}

void HeadsetInterface::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case HeadsetInterfaceCommand::REGISTER_LISTENER:
	{
		const commands::HeadsetListenerRegistration& registration = dynamic_cast<const commands::HeadsetListenerRegistration&>(args);
		if (registration.controller_number_ == 2) {
			for (unsigned char i = 0; i < 2; i++) {
				RegisterOrUnregisterActor(registration.register_not_unregister_, registration.actor_to_register_, i, registration.listener_id_);
			}
		} else {
			RegisterOrUnregisterActor(registration.register_not_unregister_, registration.actor_to_register_, registration.controller_number_, registration.listener_id_);
		}
	}
	break;
	case HeadsetInterfaceCommand::HAPTIC_PULSE:
	{
		const auto& pulse_info = dynamic_cast<const commands::HapticPulse&>(args);
		HandleHapticPulse(pulse_info.controller_number_, pulse_info.strength_, pulse_info.duration_);
	}
	break;
	case InputCommand::TICK:
		for (unsigned char i = 0; i < 2; i++) {
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
				Target(listener_groups_[i][static_cast<size_t>(ListenerId::CONTROLLER_MOVEMENT)]),
				make_unique<commands::ControllerMovement>(
					i,
					new_controller_position,
					controller_positions_[i].Delta(new_controller_position))));
			controller_positions_[i] = new_controller_position;
			scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
				Target(controller_graphics_[i]),
				make_unique<commands::ComponentPlacement>("sphere", DirectX::XMMatrixScaling(controller_sphere_radius, controller_sphere_radius, controller_sphere_radius)*controller_positions_[i].GetMatrix())));

			// Update the button state of the controller.
			HandleNewControllerState(i, headset_.GetGameControllerState(controller_index));
		}
	}
}

void HeadsetInterface::HandleNewControllerState(unsigned char hand_index, vr::VRControllerState_t new_state) {
	bool trigger_is_pulled = HeadsetHelpers::IsTriggerPulled(new_state);
	bool trigger_was_pulled = HeadsetHelpers::IsTriggerPulled(controller_states_[hand_index]);
	// If the trigger's state has changed, alert all listeners.
	if (trigger_is_pulled != trigger_was_pulled) {
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(listener_groups_[hand_index][static_cast<size_t>(ListenerId::TRIGGER_STATE_CHANGE)]),
			make_unique<commands::TriggerStateChange>(
				hand_index, controller_positions_[hand_index], trigger_is_pulled)));
	}

	vr::VRControllerAxis_t touchpad_delta;
	touchpad_delta.x = new_state.rAxis[0].x - controller_states_[hand_index].rAxis[0].x;
	touchpad_delta.y = new_state.rAxis[0].y - controller_states_[hand_index].rAxis[0].y;

	if ((vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) & new_state.ulButtonTouched) &&
		(vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) & controller_states_[hand_index].ulButtonTouched)) {
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(listener_groups_[hand_index][static_cast<size_t>(ListenerId::TOUCHPAD_SLIDE)]),
			make_unique<commands::TouchpadMotion>(
				HeadsetInterfaceCommand::LISTEN_TOUCHPAD_SLIDE,
				hand_index,
				controller_positions_[hand_index],
				new_state.rAxis[0],
				touchpad_delta)));
	}
	if ((vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) & new_state.ulButtonPressed) &&
		(vr::ButtonMaskFromId(vr::k_EButton_SteamVR_Touchpad) & controller_states_[hand_index].ulButtonPressed)) {
		scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
			Target(listener_groups_[hand_index][static_cast<size_t>(ListenerId::TOUCHPAD_DRAG)]),
			make_unique<commands::TouchpadMotion>(
				HeadsetInterfaceCommand::LISTEN_TOUCHPAD_DRAG,
				hand_index,
				controller_positions_[hand_index],
				new_state.rAxis[0],
				touchpad_delta)));
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
				VertexType::vertex_type_all,
				false)}
		},
		{});
	sphere_details.heirarchy_.shader_file_definition_ = game_scene::actors::ShaderFileDefinition("solidcolor.hlsl");
	sphere_details.heirarchy_.vertex_shader_input_type_ = VertexType::vertex_type_texture;
	sphere_details.heirarchy_.entity_group_ = "basic";
	sphere_details.heirarchy_.shader_settings_ = {
		{1.0f, 0.0f, 0.5f},
	};

	ActorId new_controller = scene_->AddActor(make_unique<game_scene::actors::GraphicsObject>());
	CommandQueueLocation created_actor =
	scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(Target(new_controller), 
		make_unique<game_scene::WrappedCommandArgs<game_scene::actors::GraphicsObjectDetails>>(
			game_scene::GraphicsObjectCommand::CREATE_COMPONENTS,
			sphere_details)));
	scene_->MakeCommandAfter(created_actor, Command(
		game_scene::Target(new_controller),
		make_unique<game_scene::commands::ComponentPlacement>("sphere", DirectX::XMMatrixScaling(controller_sphere_radius, controller_sphere_radius, controller_sphere_radius))));
	return new_controller;
}

void HeadsetInterface::HandleHapticPulse(unsigned char hand_index, uint32_t strength, uint32_t duration) {
	auto controller_index = headset_.GetDeviceIndex(vr::TrackedDeviceClass_Controller, hand_index);
	headset_.SetControllerHaptic(controller_index, strength);
}

}  // actors
}  // game_scene
