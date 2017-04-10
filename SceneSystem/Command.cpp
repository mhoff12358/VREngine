#include "stdafx.h"
#include "Command.h"

namespace game_scene {
Command& Command::operator=(Command&& other)
{
	swap(*this, other);
	return *this;
}

const Target& Command::GetTarget() const {
	return target_;
}

CommandArgs& Command::GetArgs() {
	if (unowned_args_) {
		return *unowned_args_;
	}
	return *owned_args_;
}

}  // game_scene
