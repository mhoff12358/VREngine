#include "stdafx.h"
#include "NichijouGraph.h"

#include "Scene.h"
#include "QueryArgs.h"

namespace game_scene {
namespace actors {

void NichijouGraph::AddedToScene() {
	try {
		object main_namespace = import("__main__").attr("__dict__");
		exec("import scripts.nichijou_graph as nichijou_graph", main_namespace);
		object loaded_module = main_namespace["nichijou_graph"];
		graph_ = loaded_module.attr("load_graph")();

		object vertices = graph_["vertices"];
		for (ssize_t i = 0; i < len(vertices); i++) {
			string value = extract<string>(vertices[i]["name"]);
			std::cout << "Vertex " << i << ": " << value << std::endl;
		}
	} catch (error_already_set) {
		PyErr_Print();
	}
}

}  // actors
}  // game_scene
