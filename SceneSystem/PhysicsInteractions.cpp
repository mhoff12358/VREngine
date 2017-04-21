#pragma once

#include "stdafx.h"

#include "PhysicsInteractions.h"

FORCE_LINK_THIS(PhysicsInteractions)

namespace game_scene {
namespace commands {

REGISTER_COMMAND(PhysicsInteractionCommand, RIGID_BODY_MOVED);

RigidBodyUpdated::RigidBodyUpdated() : CommandArgs(PhysicsInteractionCommand::RIGID_BODY_MOVED) {}

}  // commands
}  // game_scene
