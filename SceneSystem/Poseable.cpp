#include "stdafx.h"

#include "Poseable.h"

FORCE_LINK_THIS(Poseable)

namespace game_scene {
namespace commands {

REGISTER_COMMAND(PoseableCommand, ACCEPT_NEW_POSE);
REGISTER_COMMAND(PoseableCommand, PUSH_NEW_POSE);


}  // commands
}  // game_scene
