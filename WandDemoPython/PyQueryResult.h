#pragma once
#include "stdafx.h"

#include "SceneSystem/stdafx.h"

#include "SceneSystem/BoostPythonWrapper.h"
#include "SceneSystem/QueryResult.h"

struct PyQueryResult : public game_scene::QueryResult/*, boost::python::wrapper<game_scene::QueryResult>*/ {
public:
	object self_;

	PyQueryResult(object self, game_scene::IdType type) : game_scene::QueryResult(type), self_(self) {}

	static std::auto_ptr<PyQueryResult> Factory(object self, game_scene::IdType type) {
		return std::auto_ptr<PyQueryResult>(make_unique<PyQueryResult>(self, type).release());
	}

};
