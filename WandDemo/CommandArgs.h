#pragma once

#include "stdafx.h"

namespace game_scene {

class CommandType {
public:
	CommandType(int id) : id_(id) {}

	enum CommandTypeId : int {
		ADDED_TO_SCENE = 1,
		INPUT = 100,
		GRAPHICS = 200,
		HEADSET_INTERFACE = 300,
		NICHIJOU_GRAPH = 400,
	};

	int Type() {return id_;}
	int id_;
};

class CommandArgs {
public:
	CommandArgs(CommandType type);
	virtual ~CommandArgs();

	int Type() const {return type_.id_;}

private:
	CommandType type_;
};

template <typename WrappedType>
class WrappedCommandArgs : public CommandArgs {
public:
	WrappedCommandArgs(CommandType type, WrappedType data)
		: CommandArgs(type), data_(data) {}

	WrappedType data_;
};

}  // game_scene