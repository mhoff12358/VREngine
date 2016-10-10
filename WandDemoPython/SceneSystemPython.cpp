#pragma once
#include "stdafx.h"

#include "boost/python.hpp"
#include "SceneSystem/QueryArgs.h"
#include "SceneSystem/QueryResult.h"
#include "SceneSystem/CommandArgs.h"
#include "SceneSystem/Registry.h"
#include "SceneSystem/InputCommandArgs.h"
#include "SceneSystem/HeadsetInterface.h"
#include "SceneSystem/IOInterface.h"
#include "SceneSystem/Scene.h"
#include "SceneSystem/GraphicsResources.h"
#include "VRBackend/PipelineCamera.h"
#include "VRBackend/Pose.h"
#include "VRBackend/EntityHandler.h"

#include "PyActor.h"
#include "PyScene.h"

#define BOOST_PTR_MAGIC(class_name) \
namespace boost { \
	template <> \
	class_name const volatile * get_pointer<class class_name const volatile>( \
		class class_name const volatile *c) \
	{ return c; } \
}

BOOST_PTR_MAGIC(PyActor)
BOOST_PTR_MAGIC(game_scene::CommandArgs)
BOOST_PTR_MAGIC(game_scene::QueryArgs)
BOOST_PTR_MAGIC(game_scene::QueryResult)
BOOST_PTR_MAGIC(game_scene::actors::GraphicsResources)
BOOST_PTR_MAGIC(game_scene::commands::IOListenerRegistration)

int x(const array<int, 2>& arr) { return arr[0]; }
int y(const array<int, 2>& arr) { return arr[1]; }

int getitem_foo(array<int, 2>& foo, int index) {
	return foo[index];
}

game_scene::actors::GraphicsResources& GetGraphicsResources(game_scene::QueryResultWrapped<game_scene::actors::GraphicsResources&>& a) {
	return a.data_;
}

BOOST_PYTHON_MODULE(scene_system_) {
	using namespace boost::python;
	class_<PyActor, boost::noncopyable>("Actor")
		.def("HandleCommand", &game_scene::Shmactor::HandleCommand, &PyActor::default_HandleCommand)
		.def("AddedToScene", &game_scene::Shmactor::AddedToScene, &PyActor::default_AddedToScene)
		.def("AnswerQuery", &PyActor::PyAnswerQuery)
		.def("EmbedSelf", &PyActor::EmbedSelf)
		.def("GetScene", &game_scene::Shmactor::GetScene, return_value_policy<reference_existing_object>())
		.add_property("id", &PyActor::GetId)
		.def("SetScene", &game_scene::Shmactor::SetScene);

	class_<game_scene::QueryArgs, boost::noncopyable>("QueryArgs", init<game_scene::IdType>())
		.def("Type", &game_scene::QueryArgs::Type);

	class_<game_scene::QueryResult, std::auto_ptr<game_scene::QueryResult>, boost::noncopyable>("QueryResult", init<game_scene::IdType>())
		.def("Type", &game_scene::QueryResult::Type);

	class_<game_scene::CommandArgs, std::auto_ptr<game_scene::CommandArgs>, boost::noncopyable>("CommandArgs", init<game_scene::IdType>())
		.def("Type", &game_scene::CommandArgs::Type);

	class_<game_scene::CommandQueueLocation>("CommandQueueLocation", no_init);

	class_<game_scene::ActorId>("ActorId", no_init)
		.def_readonly("id", &game_scene::ActorId::id_);

	class_<game_scene::Command, boost::noncopyable>("Command", no_init);

	class_<game_scene::Target>("Target", init<game_scene::ActorId>())
		.def("AllActors", &game_scene::Target::AllActors)
		.staticmethod("AllActors");

	auto scene_registration = class_<game_scene::Scene, boost::noncopyable>("Scene", no_init)
		.def("RegisterDependency", &game_scene::Scene::RegisterDependency)
		.def("AskQuery", &PyScene::AskQuery)
		.def("FrontOfCommands", &game_scene::Scene::FrontOfCommands)
		.def("AddActor", &PyScene::AddActor)
		.def("AddActorAfter", &PyScene::AddActorAfter)
		.def("AddActorReturnInitialize", &PyScene::AddActorReturnInitialize)
		.def("AddActorAfterReturnInitialize", &PyScene::AddActorAfterReturnInitialize)
		.def("RegisterByName", &game_scene::Scene::RegisterByName)
		.def("FindByName", &game_scene::Scene::FindByName)
		.def("AddActorToGroup", &game_scene::Scene::AddActorToGroup)
		.def("RemoveActorFromGroup", &game_scene::Scene::RemoveActorFromGroup);

	class_<game_scene::RegistryMap, boost::noncopyable>("RegistryMap", no_init)
		.def("Register", &game_scene::RegistryMap::Register)
		.def("IdFromName", &game_scene::RegistryMap::IdFromName)
		.def("LookupName", &game_scene::RegistryMap::LookupName);

	class_<game_scene::CommandType, boost::noncopyable>("CommandType", no_init)
		.def_readonly("ADDED_TO_SCENE", &game_scene::CommandType::ADDED_TO_SCENE);

	class_<game_scene::InputCommand>("InputCommand", no_init)
		.def_readonly("TICK", &game_scene::InputCommand::TICK)
		.def_readonly("INPUT_UPDATE", &game_scene::InputCommand::INPUT_UPDATE);

	class_<game_scene::commands::TimeTick, bases<game_scene::CommandArgs>, boost::noncopyable>("TimeTick", no_init)
		.def_readonly("duration", &game_scene::commands::TimeTick::duration_);

	class_<game_scene::commands::HeadsetListenerRegistration, bases<game_scene::CommandArgs>, boost::noncopyable>("ListenerRegistration", no_init)
		.def_readonly("register_not_unregister", &game_scene::commands::HeadsetListenerRegistration::register_not_unregister_)
		.def_readonly("actor_to_register", &game_scene::commands::HeadsetListenerRegistration::actor_to_register_)
		.def_readonly("listener_id", &game_scene::commands::HeadsetListenerRegistration::listener_id_);

	class_<game_scene::commands::ControllerMovement, bases<game_scene::CommandArgs>, boost::noncopyable>("ControllerMovement", no_init)
		.def_readonly("movement_vector", &game_scene::commands::ControllerMovement::movement_vector_);

	class_<game_scene::commands::ControllerInformation, bases<game_scene::CommandArgs>, boost::noncopyable>("ControllerInformation", no_init)
		.def_readonly("controller_number", &game_scene::commands::ControllerInformation::controller_number_);

	class_<game_scene::commands::TriggerStateChange, bases<game_scene::commands::ControllerInformation>, boost::noncopyable>("TriggerStateChange", no_init)
		.def_readonly("is_pulled", &game_scene::commands::TriggerStateChange::is_pulled_);

	class_<game_scene::commands::TouchpadMotion, bases<game_scene::commands::ControllerInformation>, boost::noncopyable>("TouchpadMotion", no_init)
		.def_readonly("position", &game_scene::commands::TouchpadMotion::position_)
		.def_readonly("delta", &game_scene::commands::TouchpadMotion::delta_);

#include "io_interface.ipp"
#include "graphics_resources.ipp"
#include "pose.ipp"

	void (PipelineCamera::*SetLocationFromLocation)(const Location&) = &PipelineCamera::SetLocation;
	void (PipelineCamera::*SetOrientationFromQuaternion)(const Quaternion&) = &PipelineCamera::SetOrientation;
	class_<PipelineCamera, boost::noncopyable>("PipelineCamera", no_init)
		.def("SetLocation", SetLocationFromLocation)
		.def("SetOrientation", SetOrientationFromQuaternion)
		.def("SetPose", &PipelineCamera::SetPose)
		.def("BuildMatrices", &PipelineCamera::BuildMatrices);

	int& (array<int, 2>::*at)(size_t) = &array<int, 2>::at;
	class_<array<int, 2>>("Arr2")
		.def("__getitem__", &getitem_foo);
		def("x", &x);
		def("y", &y);
}