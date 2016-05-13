#include "stdafx.h"
#include "Command.h"

namespace game_scene {
Command::Command(Command&& command) : Command(command.target_, move(command.args_)) {} 

Command& Command::operator=(Command other)
{
	swap(*this, other);
	return *this;
}

const Target& Command::GetTarget() const {
	return target_;
}

const CommandArgs& Command::GetArgs() const {
	return *args_;
}

}  // game_scene
