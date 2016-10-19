#pragma once
#include "stdafx.h"
#include "StlHelper.h"

#include "SceneSystem/QueryArgs.h"
#include "SceneSystem/QueryResult.h"
#include "SceneSystem/CommandArgs.h"
#include "SceneSystem/Registry.h"
#include "SceneSystem/InputCommandArgs.h"
#include "SceneSystem/HeadsetInterface.h"
#include "SceneSystem/IOInterface.h"
#include "SceneSystem/Scene.h"
#include "SceneSystem/GraphicsResources.h"
#include "SceneSystem/EntitySpecification.h"
#include "SceneSystem/NewGraphicsObject.h"
#include "VRBackend/PipelineCamera.h"
#include "VRBackend/Pose.h"
#include "VRBackend/EntityHandler.h"

void EntitySpecificationInterface() {
	class_<ModelIdentifier>("ModelIdentifier", init<>())
		.def(init<string>())
		.def(init<string, string>());

	class_<ShaderStages>("ShaderStages", no_init)
		.def("Vertex", &ShaderStages::Vertex)
		.staticmethod("Vertex")
		.def("Geometry", &ShaderStages::Geometry)
		.staticmethod("Geometry")
		.def("Pixel", &ShaderStages::Pixel)
		.staticmethod("Pixel")
		.def("All", &ShaderStages::All)
		.staticmethod("All")
		.def("And", &ShaderStages::and);

	CreateListToCollectionWithResize<vector<float>, game_scene::ShaderSettingsValue>(
		CreateIndexing<size_t, vector<float>, game_scene::ShaderSettingsValue>(
			class_<game_scene::ShaderSettingsValue>("ShaderSettingsValue", no_init)));

	class_<game_scene::NewShaderDetails>("ShaderDetails")
		.def(init<string, VertexType>())
		.def(init<string, VertexType, bool>())
		.def(init<string, VertexType, ShaderStages>());

	class_<game_scene::NewIndividualTextureDetails>("IndividualTextureDetails", init<string, ShaderStages, int, int>());
	CreateVector<game_scene::NewIndividualTextureDetails>("IndividualTextureDetails");

	class_<game_scene::NewModelDetails>("ModelDetails")
		.def(init<const ModelIdentifier&>());

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