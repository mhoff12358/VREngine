#pragma once

#include "stdafx.h"

#include "Actor.h"
#include "openvr.h"
#include "CommandArgs.h"
#include "VRBackend/Pose.h"

class Headset;

namespace game_scene {
namespace actors {

namespace HeadsetHelpers {
bool IsTriggerPulled(const vr::VRControllerState_t& state);
}  // HeadsetHelpers

class HeadsetInterface : public Actor {
public:
	HeadsetInterface(Headset& headset) :
		headset_(headset),
		controller_connectedness_({false, false}),
		controller_graphics_({ActorId::UnsetId, ActorId::UnsetId})
	{}

	void HandleCommand(const CommandArgs& args);
	void AddedToScene() override;

	ActorId CreateControllerActor();

	enum class ListenerId : unsigned char {
		TRIGGER_STATE_CHANGE = 0,
		TOUCHPAD_SLIDE,
		TOUCHPAD_DRAG,
		CONTROLLER_MOVEMENT,
		NUM_LISTENERS,
	};

private:
	void HandleNewControllerState(unsigned char hand_index, vr::VRControllerState_t new_state);
	void HeadsetInterface::RegisterOrUnregisterActor(bool register_not_unregister, ActorId actor, unsigned char controller_index, ListenerId listener_id);

	array<bool, 2> controller_connectedness_;
	array<Pose, 2> controller_positions_;
	array<vr::VRControllerState_t, 2> controller_states_;
	array<ActorId, 2> controller_graphics_;
	array<array<ActorId, static_cast<size_t>(ListenerId::NUM_LISTENERS)>, 2> listener_groups_;

	Headset& headset_;
};

}  // actors

class HeadsetInterfaceCommand {
public:
	DECLARE_COMMAND(HeadsetInterfaceCommand, REGISTER_LISTENER);
	DECLARE_COMMAND(HeadsetInterfaceCommand, LISTEN_TRIGGER_STATE_CHANGE);
	DECLARE_COMMAND(HeadsetInterfaceCommand, LISTEN_TOUCHPAD_SLIDE);
	DECLARE_COMMAND(HeadsetInterfaceCommand, LISTEN_TOUCHPAD_DRAG);
	DECLARE_COMMAND(HeadsetInterfaceCommand, LISTEN_CONTROLLER_MOVEMENT);
};

namespace commands {
class HeadsetListenerRegistration : public CommandArgs {
public:
	HeadsetListenerRegistration(
		bool register_not_unregister,
		ActorId actor_to_register,
		actors::HeadsetInterface::ListenerId listener_id,
		unsigned char controller_number = 2) : 
		CommandArgs(HeadsetInterfaceCommand::REGISTER_LISTENER),
		register_not_unregister_(register_not_unregister),
		actor_to_register_(actor_to_register),
		listener_id_(listener_id),
		controller_number_(controller_number) {}

	bool register_not_unregister_;
	ActorId actor_to_register_;
	actors::HeadsetInterface::ListenerId listener_id_;
	unsigned char controller_number_;
};

class ControllerInformation : public CommandArgs {
public:
	ControllerInformation(IdType type, unsigned char controller_number, Pose controller_position) :
		CommandArgs(type),
		controller_number_(controller_number),
		controller_position_(controller_position) {}
	Pose controller_position_;
	unsigned char controller_number_;
};

class ControllerMovement : public ControllerInformation {
public:
	ControllerMovement(unsigned char controller_number, Pose controller_position, Location movement_vector) :
		ControllerInformation(HeadsetInterfaceCommand::LISTEN_CONTROLLER_MOVEMENT, controller_number, controller_position),
		movement_vector_(movement_vector) {}

	Location movement_vector_;
};

class TriggerStateChange : public ControllerInformation {
public:
	TriggerStateChange(unsigned char controller_number, Pose controller_position, bool is_pulled) :
		ControllerInformation(HeadsetInterfaceCommand::LISTEN_TRIGGER_STATE_CHANGE, controller_number, controller_position),
		is_pulled_(is_pulled) {}

	bool is_pulled_;
};

class TouchpadMotion : public ControllerInformation {
public:
	TouchpadMotion(IdType type, unsigned char controller_number, Pose controller_position, vr::VRControllerAxis_t position, vr::VRControllerAxis_t delta)
		: ControllerInformation(type, controller_number, controller_position), position_(position), delta_(delta) {}

	vr::VRControllerAxis_t position_;
	vr::VRControllerAxis_t delta_;
};

}  // commands
}  // game_scene