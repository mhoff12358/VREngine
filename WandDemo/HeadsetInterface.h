#pragma once

#include "stdafx.h"
#include "Shmactor.h"

#include "Headset.h"

namespace game_scene {
namespace actors {

namespace HeadsetHelpers {
bool IsTriggerPulled(const vr::VRControllerState_t& state);
}  // HeadsetHelpers

class HeadsetInterface : public Shmactor {
public:
	HeadsetInterface(Headset& headset) :
		headset_(headset),
		controller_connectedness_({false, false}),
		controller_graphics_({ActorId::UnsetId, ActorId::UnsetId})
	{}

	void HandleCommand(const CommandArgs& args);
	void AddedToScene();

	ActorId CreateControllerActor();

	enum class ListenerId : unsigned char {
		TRIGGER_STATE_CHANGE = 0,
		TOUCHPAD_SLIDE,
		TOUCHPAD_DRAG,
		CONTROLLER_MOVEMENT,
		NUM_LISTENERS,
	};

private:
	void HandleNewControllerState(int hand_index, vr::VRControllerState_t new_state);

	array<bool, 2> controller_connectedness_;
	array<Pose, 2> controller_positions_;
	array<vr::VRControllerState_t, 2> controller_states_;
	array<ActorId, 2> controller_graphics_;
	array<ActorId, static_cast<size_t>(ListenerId::NUM_LISTENERS)> listener_groups_;

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
class ListenerRegistration : public CommandArgs {
public:
	ListenerRegistration(
		bool register_not_unregister,
		ActorId actor_to_register,
		actors::HeadsetInterface::ListenerId listener_id) : 
		CommandArgs(HeadsetInterfaceCommand::REGISTER_LISTENER),
		register_not_unregister_(register_not_unregister),
		actor_to_register_(actor_to_register),
		listener_id_(listener_id) {}

	bool register_not_unregister_;
	ActorId actor_to_register_;
	actors::HeadsetInterface::ListenerId listener_id_;
};

class ControllerInformation : public CommandArgs {
public:
	ControllerInformation(IdType type, unsigned char controller_number) : CommandArgs(type), controller_number_(controller_number) {}
	unsigned char controller_number_;
};

class ControllerMovement : public ControllerInformation {
public:
	ControllerMovement(unsigned char controller_number, Location movement_vector) :
		ControllerInformation(HeadsetInterfaceCommand::LISTEN_CONTROLLER_MOVEMENT, controller_number),
		movement_vector_(movement_vector) {}

	Location movement_vector_;
};

class TriggerStateChange : public ControllerInformation {
public:
	TriggerStateChange(unsigned char controller_number, bool is_pulled) :
		ControllerInformation(HeadsetInterfaceCommand::LISTEN_TRIGGER_STATE_CHANGE, controller_number),
		is_pulled_(is_pulled) {}

	bool is_pulled_;
};

class TouchpadMotion : public ControllerInformation {
public:
	TouchpadMotion(IdType type, unsigned char controller_number, vr::VRControllerAxis_t position, vr::VRControllerAxis_t delta)
		: ControllerInformation(type, controller_number), position_(position), delta_(delta) {}

	vr::VRControllerAxis_t position_;
	vr::VRControllerAxis_t delta_;
};

}  // commands
}  // game_scene