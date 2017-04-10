#pragma once

#include "stdafx.h"
#include "Target.h"
#include "CommandArgs.h"

namespace game_scene {

class Command {
public:
	Command(const Target& target, unique_ptr<CommandArgs> args)
		: target_(target), owned_args_(move(args)), unowned_args_(nullptr) {}
	Command(const Target& target, CommandArgs* args)
		: target_(target), owned_args_(nullptr), unowned_args_() {}
	Command(Command&& command)
		: Command(command.target_, move(command.owned_args_), command.unowned_args_) {
		command.target_ = Target(ActorId(-1));
		command.unowned_args_ = nullptr;
	}
	~Command() {}
	Command(const Command& command) = delete;

	Command& operator=(Command&& other);

	const Target& GetTarget() const;
	CommandArgs& GetArgs();

private:
	Command(
		const Target& target,
		unique_ptr<CommandArgs> owned_args,
		CommandArgs* unowned_args)
		: target_(target), owned_args_(move(owned_args)), unowned_args_(unowned_args) {}

	Target target_;
	unique_ptr<CommandArgs> owned_args_;
	CommandArgs* unowned_args_;
};
}  // game_scene
