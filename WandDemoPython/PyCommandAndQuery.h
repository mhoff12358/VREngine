#pragma once
#include "stdafx.h"

#include "SceneSystem/stdafx.h"

#include "SceneSystem/BoostPythonWrapper.h"
#include "SceneSystem/QueryResult.h"
#include "SceneSystem/QueryArgs.h"
#include "SceneSystem/CommandArgs.h"


struct PyCommandArgs : public game_scene::CommandArgs {
public:
	PyCommandArgs(object self) : game_scene::CommandArgs(static_cast<game_scene::IdType>(extract<game_scene::IdType>(self.attr("Type")()))), self_(self) {}

	object self_;
};

struct PyQueryArgs : public game_scene::QueryArgs {
public:
	PyQueryArgs(object self) : game_scene::QueryArgs(static_cast<game_scene::IdType>(extract<game_scene::IdType>(self.attr("Type")()))), self_(self) {}

	object self_;
};

struct PyQueryResult : public game_scene::QueryResult {
public:
	PyQueryResult(object self) : game_scene::QueryResult(static_cast<game_scene::IdType>(extract<game_scene::IdType>(self.attr("Type")()))), self_(self) {}

	object self_;
};
