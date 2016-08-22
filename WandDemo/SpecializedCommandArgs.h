#pragma once

#include "CommandArgs.h"
#include "InputHandler.h"
#include "ResourcePool.h"

namespace game_scene {
namespace commands {

class InputCommandType : public CommandType {
public:
	enum InputCommandTypeId : int {
		TICK = 999,
		INPUT_UPDATE,
	};
};

class TimeTick : public CommandArgs {
public:
	explicit TimeTick(const int duration)
		: CommandArgs(InputCommandType::TICK), duration_(duration) {
	}

	const int duration_;
};

class InputUpdate : public CommandArgs {
public:
	explicit InputUpdate(const InputHandler* input)
		: CommandArgs(InputCommandType::INPUT_UPDATE), input_(*input) {
	}
	const InputHandler& input_;
};

}  // commands
}  // game_scene