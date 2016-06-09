#pragma once

#include "stdafx.h"

namespace game_scene {

class CommandType {
public:
	CommandType(int id) : id_(id) {}

	enum CommandTypeId : int {
		INPUT = 0,
		GRAPHICS = 100,
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
class CommandArgsWrapper : public CommandArgs {
public:
	CommandArgsWrapper(CommandType type, WrappedType data)
		: CommandArgs(type), data_(data) {}

	WrappedType data_;
};

}  // game_scene