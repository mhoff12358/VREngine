#pragma once
#include "stdafx.h"

#include "SceneSystem/KinectInterface.h"
#include "StlHelper.h"

void KinectInterface(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<game_scene::commands::BodiesDiscovered, bases<game_scene::CommandArgs>, boost::noncopyable>("BodiesDiscovered", no_init)
		.def_readonly("tracking_ids", &game_scene::commands::BodiesDiscovered::tracking_ids_);

	CreateVector<TrackingId>("TrackingId");

	class_<game_scene::KinectInterfaceCommand>("KinectInterfaceCommand")
		.def_readonly("REGISTER_LISTEN_FOR_BODIES", &game_scene::KinectInterfaceCommand::REGISTER_LISTEN_FOR_BODIES)
		.def_readonly("BODIES_DISCOVERED", &game_scene::KinectInterfaceCommand::BODIES_DISCOVERED);

	class_<game_scene::commands::ListenForBodies, bases<game_scene::CommandArgs>, std::auto_ptr<game_scene::commands::ListenForBodies>, boost::noncopyable>("ListenForBodies",
		init<game_scene::ActorId, bool>());
	scene_registration.def("MakeCommandAfter", &PyScene::MakeCommandAfter<game_scene::commands::ListenForBodies>);
}
