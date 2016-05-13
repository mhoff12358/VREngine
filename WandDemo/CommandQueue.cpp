#include "stdafx.h"
#include "CommandQueue.h"

namespace game_scene {

CommandQueue::CommandQueue() {
}

CommandQueue::~CommandQueue() {
}

CommandQueueLocation CommandQueue::InsertCommand(CommandQueueLocation new_location, Command new_command) {
	return command_queue_.insert(new_location, move(new_command));
}

bool CommandQueue::IsEmpty() const {
	return command_queue_.empty();
}

Command CommandQueue::PopActiveCommand() {
	Command active_command = move(command_queue_.back());
	command_queue_.pop_back();
	return active_command;
}

CommandQueueLocation CommandQueue::GetNewFront() {
	return command_queue_.end();
}

}  // game_scene