#include "stdafx.h"
#include "GraphicsObjectInterface.h"

#include "StlHelper.h"
#include "PyScene.h"
#include "SceneSystem/NewGraphicsObject.h"

BOOST_PTR_MAGIC(game_scene::commands::CreateNewGraphicsObject)
BOOST_PTR_MAGIC(game_scene::commands::PlaceNewComponent)
BOOST_PTR_MAGIC(game_scene::commands::SetEntityShaderValues)

void GraphicsObjectInterface(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	PyScene::AddActorSubclassCreation<game_scene::actors::NewGraphicsObject>(scene_registration, "GraphicsObject");

	class_<
		game_scene::commands::CreateNewGraphicsObject,
		bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::commands::CreateNewGraphicsObject>,
		boost::noncopyable>(
			"CreateGraphicsObject",
			init<
				string,
				vector<game_scene::EntitySpecification>,
				vector<game_scene::ComponentInfo>>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::commands::CreateNewGraphicsObject>);

	class_<
		game_scene::commands::PlaceNewComponent,
		bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::commands::PlaceNewComponent>,
		boost::noncopyable>(
			"PlaceComponent",
			init<string, Pose>());
	scene_registration.def("MakeCommandAfter", &PyScene::MakeCommandAfter<game_scene::commands::PlaceNewComponent>);

	class_<
		game_scene::commands::SetEntityShaderValues,
		bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::commands::SetEntityShaderValues>,
		boost::noncopyable>(
			"SetEntityShaderValues",
			init<string, game_scene::ShaderSettingsValue>());
	scene_registration.def("MakeCommandAfter", &PyScene::MakeCommandAfter<game_scene::commands::SetEntityShaderValues>);
}