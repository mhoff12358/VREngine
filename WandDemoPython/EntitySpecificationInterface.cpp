#include "stdafx.h"
#include "EntitySpecificationInterface.h"

#include "StlHelper.h"
#include "SceneSystem/EntitySpecification.h"
#include "SceneSystem/NewGraphicsObject.h"

void EntitySpecificationInterface() {
	class_<ModelIdentifier>("ModelIdentifier", init<>())
		.def(init<string>())
		.def(init<string, string>());

	class_<ShaderIdentifier>("ShaderIdentifier", init<>())
		.def(init<string, ShaderStage>())
		.def(init<string, ShaderStage, VertexType>())
		.def(init<string, VertexType>())
		.def(init<string, string, ShaderStage>())
		.def(init<string, string, ShaderStage, VertexType>())
		.def(init<string, string, VertexType>())
		.def("GetUnsetShader", &ShaderIdentifier::GetUnsetShader)
		.staticmethod("GetUnsetShader");
	CreateVector<ShaderIdentifier>("ShaderIdentifier");

	class_<ShaderStage>("ShaderStage", no_init)
		.def("Vertex", &ShaderStage::Vertex)
		.staticmethod("Vertex")
		.def("Geometry", &ShaderStage::Geometry)
		.staticmethod("Geometry")
		.def("Pixel", &ShaderStage::Pixel)
		.staticmethod("Pixel");

	CreateVector<ShaderStage>("ShaderStage");

	class_<ShaderStages>("ShaderStages", init<vector<ShaderStage>>())
		.def(init<ShaderStage>())
		.def("All", &ShaderStages::All)
		.staticmethod("All");

	CreateListToCollection<vector<float>, game_scene::ShaderSettingsValue>(
		CreateIndexing<size_t, vector<float>, game_scene::ShaderSettingsValue>(
			class_<game_scene::ShaderSettingsValue>("ShaderSettingsValue", no_init)));

	class_<game_scene::NewShaderDetails>("ShaderDetails")
		.def(init<const vector<ShaderIdentifier>&>());

	class_<game_scene::NewIndividualTextureDetails>("IndividualTextureDetails", init<string, ShaderStages, int, int>());
	CreateVector<game_scene::NewIndividualTextureDetails>("IndividualTextureDetails");

	class_<game_scene::NewModelDetails>("ModelDetails")
		.def(init<const ModelIdentifier&>())
		.def(init<const ModelIdentifier&, std::shared_ptr<ModelGenerator>>())
		.def(init<const ModelIdentifier&, ObjLoader::OutputFormat>());

	class_<game_scene::EntitySpecification>("EntitySpecification", init<string>())
		.def("SetShaderStages", &game_scene::EntitySpecification::SetShaderStages, boost::python::return_self<>())
		.def("SetShaderSettingsValue", &game_scene::EntitySpecification::SetShaderSettingsValue, boost::python::return_self<>())
		.def("SetShaders", &game_scene::EntitySpecification::SetShaders, boost::python::return_self<>())
		.def("SetTextures", &game_scene::EntitySpecification::SetTextures, boost::python::return_self<>())
		.def("SetModel", &game_scene::EntitySpecification::SetModel, boost::python::return_self<>())
		.def("SetComponent", &game_scene::EntitySpecification::SetComponent, boost::python::return_self<>());

	CreateVector<game_scene::EntitySpecification>("EntitySpecification");

	class_<game_scene::ComponentInfo>("ComponentInfo", init<string, string>());

	CreateVector<game_scene::ComponentInfo>("ComponentInfo");
}