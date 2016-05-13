#pragma once

#include "stdafx.h"
#include "Target.h"
#include "CommandArgs.h"

namespace game_scene {
class Command {
public:
	Command(Command&& command);
	Command(const Target& target, unique_ptr<CommandArgs> args) : target_(target), args_(move(args)) {}
	Command(const Command& command) = delete;

	Command& operator=(Command other);

	const Target& GetTarget() const;
	const CommandArgs& GetArgs() const;

private:
	Target target_;
	unique_ptr<CommandArgs> args_;
};
}  // game_scene
