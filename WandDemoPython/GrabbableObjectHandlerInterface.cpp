#include "stdafx.h"
#include "GrabbableObjectHandlerInterface.h"

#include "PyScene.h"
#include "StlHelper.h"
#include "SceneSystem/GrabbableObjectHandler.h"

BOOST_PTR_MAGIC(game_scene::AddGrabbableObject)
BOOST_PTR_MAGIC(game_scene::RemoveGrabbableObject)
BOOST_PTR_MAGIC(game_scene::EnDisableGrabbableObject)
BOOST_PTR_MAGIC(game_scene::ReposeGrabbableObject)
BOOST_PTR_MAGIC(game_scene::DropGrabbableObject)

void GrabbableObjectHandlerInterface(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<game_scene::GrabbableObjectCommand>("GrabbableObjectCommand")
		.def_readonly("OBJECT_GRABBED", &game_scene::GrabbableObjectCommand::OBJECT_GRABBED);

	class_<
		game_scene::AddGrabbableObject,
		std::auto_ptr<game_scene::AddGrabbableObject>,
		bases<game_scene::CommandArgs>,
		boost::noncopyable>(
			"AddGrabbableObject",
			init<game_scene::ActorId, vector<CollisionShape>&>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::AddGrabbableObject>);

	class_<
		game_scene::RemoveGrabbableObject,
		std::auto_ptr<game_scene::RemoveGrabbableObject>,
		bases<game_scene::CommandArgs>,
		boost::noncopyable>(
			"RemoveGrabbableObject",
			init<game_scene::ActorId>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::RemoveGrabbableObject>);

	class_<
		game_scene::EnDisableGrabbableObject,
		std::auto_ptr<game_scene::EnDisableGrabbableObject>,
		bases<game_scene::CommandArgs>,
		boost::noncopyable>(
			"EnDisableGrabbableObject",
			init<game_scene::ActorId, bool>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::EnDisableGrabbableObject>);

	class_<
		game_scene::ReposeGrabbableObject,
		std::auto_ptr<game_scene::ReposeGrabbableObject>,
		bases<game_scene::CommandArgs>,
		boost::noncopyable>(
			"ReposeGrabbableObject",
			init<game_scene::ActorId, Pose>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::ReposeGrabbableObject>);

	class_<
		game_scene::DropGrabbableObject,
		std::auto_ptr<game_scene::DropGrabbableObject>,
		bases<game_scene::CommandArgs>,
		boost::noncopyable>(
			"DropGrabbableObject",
			init<game_scene::ActorId>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::DropGrabbableObject>);

	class_<
		game_scene::ObjectGrabbed,
		bases<game_scene::commands::ControllerInformation>,
		boost::noncopyable>("ObjectGrabbed", no_init)
		.def_readonly("held", &game_scene::ObjectGrabbed::held_);
}
