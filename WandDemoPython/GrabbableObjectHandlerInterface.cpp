#include "stdafx.h"
#include "GrabbableObjectHandlerInterface.h"

#include "PyScene.h"
#include "StlHelper.h"
#include "SceneSystem/GrabbableObjectHandler.h"

BOOST_PTR_MAGIC(game_scene::AddGrabbableObject)
BOOST_PTR_MAGIC(game_scene::RemoveGrabbableObject)

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
		game_scene::ObjectGrabbed,
		bases<game_scene::commands::ControllerInformation>,
		boost::noncopyable>("ObjectGrabbed", no_init)
		.def_readonly("held", &game_scene::ObjectGrabbed::held_);
}
