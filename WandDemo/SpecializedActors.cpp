#include "stdafx.h"

#include "SpecializedActors.h"

namespace game_scene {
namespace actors {

void MovableCamera::HandleCommand(const CommandArgs& args) {
	switch (args.type_) {
	case CommandType::INPUT_UPDATE:
		HandleInput(dynamic_cast<const commands::InputUpdate&>(args));
		break;
	default:
		break;
	}
}

void MovableCamera::HandleInput(const commands::InputUpdate& args) {
	if (args.input_.GetKeyPressed('W')) {
		std::cout << "W is pressed!" << std::endl;
	}
}

}  // actors
}  // game_scene
