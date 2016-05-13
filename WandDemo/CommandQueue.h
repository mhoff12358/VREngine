#pragma once

#include "stdafx.h"
#include "Command.h"

namespace game_scene {
typedef list<Command>::iterator CommandQueueLocation;

class CommandQueue
{
public:
	CommandQueue();
	~CommandQueue();

	CommandQueueLocation InsertCommand(CommandQueueLocation new_location, Command new_command);
	bool IsEmpty() const;
	Command PopActiveCommand();
	CommandQueueLocation GetNewFront();

private:
	list<Command> command_queue_;
};
}  // game_scene