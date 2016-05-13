#pragma once

#include "stdafx.h"

namespace game_scene {
enum class CommandType {
	TICK = 0,
	INPUT_UPDATE = 1,
};

class CommandArgs
{
public:
	CommandArgs(CommandType type);
	virtual ~CommandArgs();

	CommandType type_;
};
}  // game_scene