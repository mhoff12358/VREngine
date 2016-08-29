#include "stdafx.h"
#include "CommandArgs.h"

namespace game_scene {

REGISTER_COMMAND(CommandType, ADDED_TO_SCENE);

CommandArgs::CommandArgs(CommandType type) : type_(type) {
}

CommandArgs::~CommandArgs() {
}

}  // game_scene
