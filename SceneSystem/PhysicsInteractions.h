#pragma once

#include "stdafx.h"

#include "InputCommandArgs.h"

namespace game_scene {
namespace commands {

class PhysicsInteractionCommand {
public:
	DECLARE_COMMAND(PhysicsInteractionCommand, RIGID_BODY_MOVED);
};

struct RigidBodyUpdated : public CommandArgs {
	RigidBodyUpdated() : CommandArgs(PhysicsInteractionCommand::RIGID_BODY_MOVED) {}
};

}  // commands
}  // game_scene