#include "stdafx.h"

#include "MovableCamera.h"
#include "Scene.h"
#include "GraphicsObject.h"

namespace game_scene {
namespace actors {

void MovableCamera::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case commands::InputCommandType::INPUT_UPDATE:
		HandleInput(dynamic_cast<const commands::InputUpdate&>(args));
		break;
	default:
		FailToHandleCommand(args);
		break;
	}
}

void MovableCamera::HandleInput(const commands::InputUpdate& args) {
	if (args.input_.GetKeyPressed('W')) {
	}
}


}  // actors
}  // game_scene
