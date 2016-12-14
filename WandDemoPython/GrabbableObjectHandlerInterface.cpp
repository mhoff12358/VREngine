#include "stdafx.h"
#include "GrabbableObjectHandlerInterface.h"

#include "PyScene.h"
#include "StlHelper.h"
#include "SceneSystem/CollisionCollection.h"
#include "SceneSystem/GrabbableObjectHandler.h"

BOOST_PTR_MAGIC(game_scene::AddCollideableObject)
BOOST_PTR_MAGIC(game_scene::RemoveCollideableObject)
BOOST_PTR_MAGIC(game_scene::EnDisableCollideableObject)
BOOST_PTR_MAGIC(game_scene::ReposeCollideableObject)
BOOST_PTR_MAGIC(game_scene::DropGrabbableObject)

void GrabbableObjectHandlerInterface(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<game_scene::GrabbableObjectCommand>("GrabbableObjectCommand")
		.def_readonly("OBJECT_GRABBED", &game_scene::GrabbableObjectCommand::OBJECT_GRABBED);

	class_<
		game_scene::AddCollideableObject,
		std::auto_ptr<game_scene::AddCollideableObject>,
		bases<game_scene::CommandArgs>,
		boost::noncopyable>(
			"AddCollideableObject",
			init<game_scene::ActorId, vector<CollisionShape>&>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::AddCollideableObject>);

	class_<
		game_scene::RemoveCollideableObject,
		std::auto_ptr<game_scene::RemoveCollideableObject>,
		bases<game_scene::CommandArgs>,
		boost::noncopyable>(
			"RemoveCollideableObject",
			init<game_scene::ActorId>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::RemoveCollideableObject>);

	class_<
		game_scene::EnDisableCollideableObject,
		std::auto_ptr<game_scene::EnDisableCollideableObject>,
		bases<game_scene::CommandArgs>,
		boost::noncopyable>(
			"EnDisableCollideableObject",
			init<game_scene::ActorId, bool>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::EnDisableCollideableObject>);

	class_<
		game_scene::ReposeCollideableObject,
		std::auto_ptr<game_scene::ReposeCollideableObject>,
		bases<game_scene::CommandArgs>,
		boost::noncopyable>(
			"ReposeCollideableObject",
			init<game_scene::ActorId, Pose>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::ReposeCollideableObject>);

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
