#pragma once
#include "boost/python.hpp"
#include "SceneSystem/QueryArgs.h"

BOOST_PYTHON_MODULE(wand_demo_py) {
	using namespace boost::python;
	class_<game_scene::QueryArgs>("QueryArgs")
		.def("Type", &game_scene::QueryArgs::Type);
	def("a", &game_scene::QueryArgs::a);
}