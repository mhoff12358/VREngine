#pragma once
#include "stdafx.h"

#include "SceneSystem/stdafx.h"

#include "SceneSystem/BoostPythonWrapper.h"
#include "SceneSystem/QueryResult.h"

std::auto_ptr<game_scene::QueryResult> ExtractRawQueryResult(std::auto_ptr<game_scene::QueryResult> in);

struct PyQueryResult : public game_scene::QueryResult {
public:
	object self_;

	PyQueryResult(object self, game_scene::IdType type) : game_scene::QueryResult(type), self_(self) {}
};

std::auto_ptr<game_scene::QueryResult> ExtractQueryResult(object in);
