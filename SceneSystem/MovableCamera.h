#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "CommandArgs.h"
#include "InputCommandArgs.h"

// Includes from external code that is used for behavior.
#include "VRBackend/Camera.h"

namespace game_scene {
namespace actors {

class MovableCamera : public Shmactor {
public:
	MovableCamera(Camera* camera) : camera_(camera) {}

	void HandleCommand(const CommandArgs& args) override;
	void HandleInput(const commands::InputUpdate& args);

private:
	Camera* camera_;
};


}  // actors
}  // game_scene
