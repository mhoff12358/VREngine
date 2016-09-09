#pragma once

#include "CommandArgs.h"
#include "VRBackend/InputHandler.h"
#include "VRBackend/ResourcePool.h"

namespace game_scene {

class InputCommand : public CommandType {
public:
	DECLARE_COMMAND(InputCommand, TICK);
	DECLARE_COMMAND(InputCommand, INPUT_UPDATE);
};

namespace commands {

class TimeTick : public CommandArgs {
public:
	explicit TimeTick(const int duration)
		: CommandArgs(InputCommand::TICK), duration_(duration) {
	}

	const int duration_;
};

class InputUpdate : public CommandArgs {
public:
	explicit InputUpdate(const InputHandler* input)
		: CommandArgs(InputCommand::INPUT_UPDATE), input_(*input) {
	}
	const InputHandler& input_;
};

}  // commands
}  // game_scene