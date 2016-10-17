#pragma once

PyScene::AddActorSubclassCreation<game_scene::actors::NewGraphicsObject>(scene_registration, "GraphicsObject");

class_<game_scene::commands::CreateNewGraphicsObject,
	bases<game_scene::CommandArgs>, std::auto_ptr<game_scene::commands::CreateNewGraphicsObject>, boost::noncopyable>("CreateGraphicsObject",
		init<string, vector<game_scene::EntitySpecification>, vector<game_scene::ComponentInfo>>());
scene_registration.def("MakeCommandAfter", &PyScene::MakeCommandAfter<game_scene::commands::CreateNewGraphicsObject>);

class_<game_scene::commands::PlaceNewComponent,
	bases<game_scene::CommandArgs>, std::auto_ptr<game_scene::commands::PlaceNewComponent>, boost::noncopyable>("PlaceComponent",
		init<string, Pose>());
scene_registration.def("MakeCommandAfter", &PyScene::MakeCommandAfter<game_scene::commands::PlaceNewComponent>);