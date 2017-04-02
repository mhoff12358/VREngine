#include "stdafx.h"

#include "InputCommandArgs.h"

namespace game_scene {

REGISTER_COMMAND(InputCommand, TICK);
REGISTER_COMMAND(InputCommand, INPUT_UPDATE);

namespace commands {

TimeTick::TimeTick(const int duration)
	: CommandArgs(InputCommand::TICK), duration_(duration) {
}

InputUpdate::InputUpdate(const InputHandler* input)
	: CommandArgs(InputCommand::INPUT_UPDATE), input_(*input) {
}

}  // commands

}  // game_scene