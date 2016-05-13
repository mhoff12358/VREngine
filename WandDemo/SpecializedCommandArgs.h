#pragma once

#include "CommandArgs.h"
#include "InputHandler.h"

namespace game_scene {
namespace commands {
class TimeTick : public CommandArgs {
public:
	explicit TimeTick(const int duration)
		: CommandArgs(CommandType::TICK), duration_(duration) {
	}

	const int duration_;
};

class InputUpdate : public CommandArgs {
public:
	explicit InputUpdate(const InputHandler* input)
		: CommandArgs(CommandType::INPUT_UPDATE), input_(*input) {
	}
	const InputHandler& input_;
};
}  // commands
}  // game_scene