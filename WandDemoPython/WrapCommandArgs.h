#pragma once

#include "boost/python.hpp"
#include "SceneSystem/CommandArgs.h"

struct WrappedCommandArgs : game_scene::CommandArgs, boost::python::wrapper<game_scene::CommandArgs> {

};

class CommandArgs {
public:
	CommandArgs(CommandType type);
	virtual ~CommandArgs();

	IdType Type() const { return type_.id_; }

private:
	CommandType type_;
};