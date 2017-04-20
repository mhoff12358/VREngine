#pragma once

#include "stdafx.h"
#include "Registry.h"

namespace game_scene {

class CommandType {
public:
	CommandType(IdType id) : id_(id) {}
	CommandType(const CommandRegistry& id) : id_(id.value_) {}

#pragma warning(push)
#pragma warning(disable: 4307)
	DECLARE_COMMAND(CommandType, ADDED_TO_SCENE);
#pragma warning(pop)

	IdType Type() {return id_;}
	IdType id_;
	string Name() const { return CommandRegistry::GetRegistry().LookupNameConst(id_); }
};

class CommandArgs {
public:
	CommandArgs(CommandType type);
	virtual ~CommandArgs();
	CommandArgs(const CommandArgs&) = delete;
	CommandArgs operator=(const CommandArgs&) = delete;

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