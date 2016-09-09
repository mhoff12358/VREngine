#pragma once
#include "stdafx.h"

#include "boost/python.hpp"
#include "SceneSystem/QueryArgs.h"
#include "SceneSystem/QueryResult.h"
#include "SceneSystem/CommandArgs.h"
#include "SceneSystem/Registry.h"

#include "PyActor.h"
#include "PyScene.h"

namespace boost
{
	template <>
	game_scene::QueryResult const volatile * get_pointer<class game_scene::QueryResult const volatile >(
		class game_scene::QueryResult const volatile *c)
	{
		return c;
	}
}
namespace boost
{
	template <>
	PyActor const volatile * get_pointer<class PyActor const volatile >(
		class PyActor const volatile *c)
	{
		return c;
	}
}

BOOST_PYTHON_MODULE(scene_system_) {
	using namespace boost::python;
	class_<PyActor, std::auto_ptr<PyActor>, boost::noncopyable>("Actor")
		.def("HandleCommand", &game_scene::Shmactor::HandleCommand, &PyActor::default_HandleCommand)
		.def("AddedToScene", &game_scene::Shmactor::AddedToScene, &PyActor::default_AddedToScene)
		.def("AnswerQuery", &PyActor::PyAnswerQuery)
		.def("EmbedSelf", &PyActor::EmbedSelf)
		.def("GetScene", &PyActor::GetScene, return_value_policy<reference_existing_object>())
		.staticmethod("GetScene")
		.def("SetScene", &game_scene::Shmactor::SetScene)
		.staticmethod("SetScene");

	class_<game_scene::QueryArgs>("QueryArgs", init<game_scene::IdType>())
		.def("Type", &game_scene::QueryArgs::Type);

	class_<game_scene::QueryResult, std::auto_ptr<game_scene::QueryResult>, boost::noncopyable>("QueryResult", init<game_scene::IdType>())
		.def("Type", &game_scene::QueryResult::Type);

	class_<game_scene::CommandArgs>("CommandArgs", init<game_scene::IdType>())
		.def("Type", &game_scene::CommandArgs::Type);

	class_<game_scene::CommandQueueLocation>("CommandQueueLocation", no_init);

	class_<game_scene::ActorId>("ActorId", no_init)
		.def_readonly("id", &game_scene::ActorId::id_);

	class_<game_scene::Command, boost::noncopyable>("Command", no_init);

	class_<game_scene::Target>("Target", init<game_scene::ActorId>())
		.def("AllActors", &game_scene::Target::AllActors)
		.staticmethod("AllActors");

	class_<game_scene::Scene, boost::noncopyable>("Scene", no_init)
		.def("RegisterDependency", &game_scene::Scene::RegisterDependency)
		.def("MakeCommandAfter", &PyScene::MakeCommandAfter)
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
}