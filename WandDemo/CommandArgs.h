#pragma once

#include "stdafx.h"
#include "Registry.h"

namespace game_scene {

class CommandType {
public:
	CommandType(IdType id) : id_(id) {}
	CommandType(const CommandRegistry& id) : id_(id.value_) {}

	DECLARE_COMMAND(CommandType, ADDED_TO_SCENE);

	IdType Type() {return id_;}
	IdType id_;
};

class CommandArgs {
public:
	CommandArgs(CommandType type);
	virtual ~CommandArgs();

	IdType Type() const {return type_.id_;}

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