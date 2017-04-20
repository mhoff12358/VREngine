#include "stdafx.h"
#include "CommandArgs.h"

namespace game_scene {

REGISTER_COMMAND(CommandType, ADDED_TO_SCENE);

CommandArgs::CommandArgs(CommandType type) : type_(type) {
	RegistryMap& registry = CommandRegistry::GetRegistry();
	if (registry.LookupName(type.id_) == "") {
		std::cout << "derp" << std::endl;
	}
}

CommandArgs::~CommandArgs() {
}

}  // game_scene
