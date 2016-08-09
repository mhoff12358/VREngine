#pragma once

#include "stdafx.h"
#include "Shmactor.h"

#include "Headset.h"

namespace game_scene {
namespace commands {
class HeadsetInterfaceCommandType : public CommandType {
public:
	enum HeadsetInterfaceCommandTypeId : int {
		REGISTER_CONTROLLER_LISTENER = HEADSET_INTERFACE,
		LISTEN_CONTROLLER_MOVEMENT,
	};
};
}  // commands

namespace actors {

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

private:
	array<bool, 2> controller_connectedness_;
	array<Pose, 2> controller_positions_;
	array<ActorId, 2> controller_graphics_;
	ActorId controller_listeners_;

	Headset& headset_;
};

}  // actors
}  // game_scene