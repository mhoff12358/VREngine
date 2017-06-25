#include "stdafx.h"

#include "PhysicsObject.h"

FORCE_LINK_THIS(PhysicsObject)

namespace game_scene {
REGISTER_COMMAND(PhysicsObjectCommand, ADD_RIGID_BODY);
REGISTER_COMMAND(PhysicsObjectCommand, REMOVE_RIGID_BODY);
REGISTER_COMMAND(PhysicsObjectCommand, ADD_UPDATED_CALLBACK);
REGISTER_QUERY(PhysicsObjectQuery, GET_RIGID_BODIES);

}  // game_scene