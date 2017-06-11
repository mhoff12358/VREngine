#include "stdafx.h"
#include "Poseable.h"

#include "StlHelper.h"
#include "PyCommandAndQuery.h"
#include "PyScene.h"
#include "SceneSystem/Poseable.h"
#include "SceneSystem/Actor.h"

BOOST_PTR_MAGIC(game_scene::commands::AcceptNewPose)
BOOST_PTR_MAGIC(game_scene::commands::PushNewPose)

void Poseable(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
  enum_<unsigned char>("FreezeBits")
    .value("LOCATION", game_scene::actors::PoseData::LOCATION)
    .value("ORIENTATION", game_scene::actors::PoseData::ORIENTATION)
    .value("SCALE", game_scene::actors::PoseData::SCALE);
  class_<game_scene::actors::PoseData>("PoseData", init<Pose>())
    .def(init<Pose, Pose, unsigned char>());

	class_<game_scene::commands::PoseableCommand, boost::noncopyable>("PoseableCommand", no_init)
		.def_readonly("ACCEPT_NEW_POSE", &game_scene::commands::PoseableCommand::ACCEPT_NEW_POSE)
		.def_readonly("PUSH_NEW_POSE", &game_scene::commands::PoseableCommand::PUSH_NEW_POSE)
		.def_readonly("GET_POSE", &game_scene::commands::PoseableCommand::GET_POSE);

  class_<
    game_scene::commands::AcceptNewPose,
    bases<game_scene::CommandArgs>,
    std::auto_ptr<game_scene::commands::AcceptNewPose>,
    boost::noncopyable>(
      "AcceptNewPose",
      init<string, string, Pose, Pose>())
    .add_property("name", &game_scene::commands::AcceptNewPose::name_)
    .add_property("pose_source", &game_scene::commands::AcceptNewPose::pose_source_)
    .add_property("new_pose", &game_scene::commands::AcceptNewPose::new_pose_)
    .add_property("old_pose", &game_scene::commands::AcceptNewPose::old_pose_);
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::commands::AcceptNewPose>);

  class_<
    game_scene::commands::PushNewPose,
    bases<game_scene::CommandArgs>,
    std::auto_ptr<game_scene::commands::PushNewPose>,
    boost::noncopyable>(
      "PushNewPose",
      init<string, string, Pose>())
    .add_property("name", &game_scene::commands::PushNewPose::name_)
    .add_property("pose_source", &game_scene::commands::PushNewPose::pose_source_)
    .add_property("new_pose", &game_scene::commands::PushNewPose::new_pose_);
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::commands::PushNewPose>);

  class_<
    game_scene::commands::GetPoseQuery,
    bases<game_scene::QueryArgs>,
    boost::noncopyable>(
      "GetPoseQuery",
      init<string>());
  RegisterQueryResultWrapped<Pose>("Pose");
}
