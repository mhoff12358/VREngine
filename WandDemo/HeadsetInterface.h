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
		CONTROLLER_MOVEMENT,
		NUM_LISTENERS,
	};

private:
	array<bool, 2> controller_connectedness_;
	array<Pose, 2> controller_positions_;
	array<vr::VRControllerState_t, 2> controller_states_;
	array<ActorId, 2> controller_graphics_;
	array<ActorId, static_cast<size_t>(ListenerId::NUM_LISTENERS)> listener_groups_;

	Headset& headset_;
};

}  // actors

namespace commands {
class HeadsetInterfaceCommandType : public CommandType {
public:
	enum HeadsetInterfaceCommandTypeId : int {
		REGISTER_LISTENER = HEADSET_INTERFACE,
		LISTEN_TRIGGER_STATE_CHANGE,
		LISTEN_CONTROLLER_MOVEMENT,
	};
};

class ListenerRegistration : public CommandArgs {
public:
	ListenerRegistration(
		bool register_not_unregister,
		ActorId actor_to_register,
		actors::HeadsetInterface::ListenerId listener_id) : 
		CommandArgs(HeadsetInterfaceCommandType::REGISTER_LISTENER),
		register_not_unregister_(register_not_unregister),
		actor_to_register_(actor_to_register),
		listener_id_(listener_id) {}

	bool register_not_unregister_;
	ActorId actor_to_register_;
	actors::HeadsetInterface::ListenerId listener_id_;
};

class ControllerMovement : public CommandArgs {
public:
	ControllerMovement(unsigned char controller_number, Location movement_vector) :
		CommandArgs(HeadsetInterfaceCommandType::LISTEN_CONTROLLER_MOVEMENT),
		controller_number_(controller_number),
		movement_vector_(movement_vector) {}

	unsigned char controller_number_;
	Location movement_vector_;
};

class TriggerStateChange : public CommandArgs {
public:
	TriggerStateChange(unsigned char controller_number, bool is_pulled) :
		CommandArgs(HeadsetInterfaceCommandType::LISTEN_TRIGGER_STATE_CHANGE),
		controller_number_(controller_number),
		is_pulled_(is_pulled) {}

	unsigned char controller_number_;
	bool is_pulled_;
};
}  // commands
}  // game_scene