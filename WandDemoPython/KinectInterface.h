#pragma once
#include "stdafx.h"

#include "SceneSystem/KinectInterface.h"
#include "VRBackend/Body.h"
#include "StlHelper.h"

void KinectInterface(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<game_scene::actors::KinectInterface, boost::noncopyable>("KinectInterface", no_init)
		.def("GetBodyLookup", &game_scene::actors::KinectInterface::GetBodyLookup)
		.staticmethod("GetBodyLookup");

	class_<game_scene::commands::BodiesDiscovered, bases<game_scene::CommandArgs>, boost::noncopyable>("BodiesDiscovered", no_init)
		.def_readonly("tracking_ids", &game_scene::commands::BodiesDiscovered::tracking_ids_);

	CreateVector<TrackingId>("TrackingId");

	class_<game_scene::KinectInterfaceCommand>("KinectInterfaceCommand")
		.def_readonly("REGISTER_LISTEN_FOR_BODIES", &game_scene::KinectInterfaceCommand::REGISTER_LISTEN_FOR_BODIES)
		.def_readonly("BODIES_DISCOVERED", &game_scene::KinectInterfaceCommand::BODIES_DISCOVERED);

	class_<game_scene::commands::ListenForBodies, bases<game_scene::CommandArgs>, std::auto_ptr<game_scene::commands::ListenForBodies>, boost::noncopyable>("ListenForBodies",
		init<game_scene::ActorId, bool>());
	scene_registration.def("MakeCommandAfter", &PyScene::MakeCommandAfter<game_scene::commands::ListenForBodies>);

	class_<game_scene::BodyLookup>("BodyLookup", no_init)
		.def("GetBody", &game_scene::BodyLookup::GetBody, return_value_policy<reference_existing_object>());

	class_<Body, boost::noncopyable>("Body", no_init)
		.def_readonly("filled", &Body::filled_)
		.def_readonly("hands", &Body::hands_)
		.def_readonly("hand_confidences", &Body::hand_confidences_)
		.def_readonly("joints", &Body::joints_)
		.def_readonly("joint_orientations", &Body::joint_orientations_);

}
