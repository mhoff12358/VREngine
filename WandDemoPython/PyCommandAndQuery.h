#pragma once
#include "stdafx.h"

#include "SceneSystem/stdafx.h"

#include "SceneSystem/BoostPythonWrapper.h"
#include "SceneSystem/QueryResult.h"
#include "SceneSystem/QueryArgs.h"
#include "SceneSystem/CommandArgs.h"

template <typename Wrapped>
void RegisterQueryResultWrapped(string wrapped_name) {
  wrapped_name = string("QueryResultWrapped_") + wrapped_name;
  class_<game_scene::QueryResultWrapped<Wrapped>, bases<game_scene::QueryResult>, boost::noncopyable>(wrapped_name.c_str(), no_init)
    .def("GetData", &game_scene::QueryResultWrapped<Wrapped>::GetData, return_value_policy<reference_existing_object>());
}

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
