#include "stdafx.h"

#include "PhysicsObject.h"

FORCE_LINK_THIS(PhysicsObject)

namespace game_scene {
namespace commands {
REGISTER_COMMAND(PhysicsObjectCommand, ADD_RIGID_BODY);
REGISTER_COMMAND(PhysicsObjectCommand, REMOVE_RIGID_BODY);
REGISTER_COMMAND(PhysicsObjectCommand, TOGGLE_RIGID_BODY);
REGISTER_COMMAND(PhysicsObjectCommand, ADD_UPDATED_CALLBACK);
}  // commands
namespace queries {
REGISTER_QUERY(PhysicsObjectQuery, GET_RIGID_BODIES);

GetRigidBodies::GetRigidBodies() : QueryArgs(PhysicsObjectQuery::GET_RIGID_BODIES) {}
}  // queries
}  // game_scene