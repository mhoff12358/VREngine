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
#include "SceneSystem/EntitySpecification.h"
#include "SceneSystem/NewGraphicsObject.h"
#include "SceneSystem/PhysicsSimulation.h"
#include "SceneSystem/Poseable.h"
#include "VRBackend/PipelineCamera.h"
#include "VRBackend/Pose.h"
#include "VRBackend/EntityHandler.h"
#include "VRBackend/ResourceIdentifier.h"

#include "PyActor.h"
#include "PyScene.h"
#include "PyCommandAndQuery.h"
#include "StlHelper.h"
#include "PythonClassHelpers.h"

#include "StlInterface.h"
#include "EntitySpecificationInterface.h"
#include "GraphicsObjectInterface.h"
#include "HeadsetInterface.h"
#include "IOInterfaceInterface.h"
#include "GraphicsResourcesInterface.h"
#include "PoseInterface.h"
#include "ModelCreationInterface.h"
#include "GrabbableObjectHandlerInterface.h"
#include "CollisionShapeInterface.h"
#include "LightInterface.h"
#include "Physics.h"
#include "Bullet.h"

BOOST_PTR_MAGIC(game_scene::ActorAdapter<PyActorImpl<game_scene::ActorImpl>>)
BOOST_PTR_MAGIC(game_scene::ActorImpl)
BOOST_PTR_MAGIC(game_scene::IActor)
BOOST_PTR_MAGIC(game_scene::CommandArgs)
BOOST_PTR_MAGIC(game_scene::QueryArgs)
BOOST_PTR_MAGIC(game_scene::QueryResult)
BOOST_PTR_MAGIC_STRUCT(PyCommandArgs)
BOOST_PTR_MAGIC_STRUCT(PyQueryArgs)
BOOST_PTR_MAGIC_STRUCT(PyQueryResult)
BOOST_PTR_MAGIC(game_scene::ActorAdapter<game_scene::actors::GraphicsResourcesImpl>)
BOOST_PTR_MAGIC(game_scene::commands::IOListenerRegistration)

object a(boost::python::tuple args, boost::python::dict kwargs) {
	auto b = boost::python::len(args);
	return object();
}

template<typename c>
class Fn {
public:
	static void Fna() {
		std::cout << c::thing() << std::endl;
	}
};

void Fn3() {

}

template <void(*a)()>
void Fn2() {
	a();
}

template <template<typename> typename a, typename b>
void Fn4() {
	a<b>::Fna();
}

class JIFOE {
public:
	static const char* thing() {
		return "FJEIOFJOIE";
	}
};

BOOST_PYTHON_MODULE(scene_system_) {
	Fn2<Fn3>();
	Fn4<Fn, JIFOE>();
	class_<game_scene::ActorImpl, boost::noncopyable>("ActorImpl", init<>());

	CreatePyActors<CreatePyActor, game_scene::ActorImpl, game_scene::actors::NewGraphicsObjectTemp>();
	CreatePyActor<game_scene::actors::PhysicsSimulation<game_scene::ActorImpl>>::Create();
	CreatePyActor<game_scene::actors::PhysicsObject<game_scene::ActorImpl>>::Create();
	CreatePyActor<game_scene::actors::PhysicsObjectCollection<game_scene::ActorImpl>>::Create();
	CreatePyActor<game_scene::actors::PhysicsObject<game_scene::actors::Poseable<game_scene::ActorImpl>>>::Create();
	CreatePyActor<game_scene::actors::PhysicsObjectCollection<game_scene::actors::Poseable<game_scene::ActorImpl>>>::Create();

	CreatePyActor<game_scene::actors::PrintNewPoses<game_scene::actors::PhysicsObject<game_scene::actors::Poseable<game_scene::ActorImpl>>>>::Create();

//	std::true_type a = game_scene::poseable::IsPoseable<game_scene::actors::PrintNewPoses<game_scene::actors::PhysicsObject<game_scene::actors::Poseable<game_scene::ActorImpl>>>>::type();
//	game_scene::poseable::IsPoseable<game_scene::actors::PrintNewPoses<game_scene::actors::PhysicsObject<game_scene::actors::Poseable<game_scene::ActorImpl>>>>::SFINAE<game_scene::actors::PrintNewPoses<game_scene::actors::PhysicsObject<game_scene::actors::Poseable<game_scene::ActorImpl>>>, game_scene::actors::PrintNewPoses<game_scene::actors::PhysicsObject<game_scene::actors::Poseable<game_scene::ActorImpl>>>::PushNewPoseImpl>
//	std::true_type b = game_scene::poseable::IsPoseable<game_scene::actors::Poseable<game_scene::ActorImpl>>::type();
	auto c = &game_scene::actors::Poseable<game_scene::ActorImpl >::PushNewPoseImpl;
	auto d = &game_scene::actors::PhysicsObject<game_scene::actors::Poseable<game_scene::ActorImpl >>::PushNewPoseImpl;
	void(game_scene::actors::Poseable<game_scene::ActorImpl>::*e)(pair<string, Pose>) = &game_scene::actors::PrintNewPoses<game_scene::actors::PhysicsObject<game_scene::actors::Poseable<game_scene::ActorImpl>>>::PushNewPoseImpl;

	class_<game_scene::CommandArgs, std::auto_ptr<game_scene::CommandArgs>, boost::noncopyable>("RawCommandArgs", init<game_scene::IdType>())
		.def("Type", &game_scene::CommandArgs::Type)
		.def("Name", &game_scene::CommandArgs::Name);

	class_<game_scene::QueryArgs, std::auto_ptr<game_scene::QueryArgs>, boost::noncopyable>("RawQueryArgs", init<game_scene::IdType>())
		.def("Type", &game_scene::QueryArgs::Type)
		.def("Name", &game_scene::QueryArgs::Name);

	class_<game_scene::QueryResult, std::auto_ptr<game_scene::QueryResult>, boost::noncopyable>("RawQueryResult", init<game_scene::IdType>())
		.def("Type", &game_scene::QueryResult::Type)
		.def("Name", &game_scene::QueryResult::Name);

	class_<game_scene::CommandQueueLocation>("CommandQueueLocation", no_init);

	class_<game_scene::ActorId>("ActorId", no_init)
		.def_readonly("id", &game_scene::ActorId::id_);

	class_<game_scene::Command, boost::noncopyable>("Command", no_init);

	class_<game_scene::Target>("Target", init<game_scene::ActorId>())
		.def("AllActors", &game_scene::Target::AllActors)
		.staticmethod("AllActors");

	auto scene_registration = class_<game_scene::Scene, boost::noncopyable>("Scene", no_init)
		.def("RegisterDependency", &game_scene::Scene::RegisterDependency)
		.def("FrontOfCommands", &game_scene::Scene::FrontOfCommands)
		.def("BackOfNewCommands", &game_scene::Scene::BackOfNewCommands)
		.def("AddActor", &PyScene::AddActor)
		.def("AddActorAfter", &PyScene::AddActorAfter)
		.def("AddActorReturnInitialize", &PyScene::AddActorReturnInitialize)
		.def("AddActorAfterReturnInitialize", &PyScene::AddActorAfterReturnInitialize)
		.def("RegisterByName", &game_scene::Scene::RegisterByName)
		.def("FindByName", &game_scene::Scene::FindByName)
		.def("AddActorToGroup", &game_scene::Scene::AddActorToGroup)
		.def("RemoveActorFromGroup", &game_scene::Scene::RemoveActorFromGroup)
		.def("AskQuery", &PyScene::AskQuery)
		.def("AskQuery", &PyScene::AskQueryWithPythonArgs)
		.def("MakeCommandAfter", &PyScene::MakeCommandAfter<game_scene::CommandArgs>)
		.def("MakeCommandAfter", &PyScene::MakeCommandAfterWithPythonArgs);

	class_<game_scene::RegistryMap, boost::noncopyable>("RegistryMap", no_init)
		.def("GetCommandRegistry", &game_scene::CommandRegistry::GetRegistry, return_value_policy<reference_existing_object>())
		.def("GetQueryRegistry", &game_scene::QueryRegistry::GetRegistry, return_value_policy<reference_existing_object>())
		.def("SetCommandRegistry", &game_scene::CommandRegistry::SetRegistry)
		.def("SetQueryRegistry", &game_scene::QueryRegistry::SetRegistry)
		.def("Register", &game_scene::RegistryMap::Register)
		.def("IdFromName", &game_scene::RegistryMap::IdFromName)
		.def("LookupName", &game_scene::RegistryMap::LookupName)
		.def("AddRegistrationsFrom", &game_scene::RegistryMap::AddRegistrationsFrom)
		.def("Size", &game_scene::RegistryMap::size);

	class_<game_scene::CommandType, boost::noncopyable>("CommandType", no_init)
		.def_readonly("ADDED_TO_SCENE", &game_scene::CommandType::ADDED_TO_SCENE);

	class_<game_scene::InputCommand>("InputCommand", no_init)
		.def_readonly("TICK", &game_scene::InputCommand::TICK)
		.def_readonly("INPUT_UPDATE", &game_scene::InputCommand::INPUT_UPDATE);

	class_<game_scene::commands::TimeTick, bases<game_scene::CommandArgs>, boost::noncopyable>("TimeTick", no_init)
		.def_readonly("duration", &game_scene::commands::TimeTick::duration_);

	void (PipelineCamera::*SetLocationFromLocation)(const Location&) = &PipelineCamera::SetLocation;
	void (PipelineCamera::*SetOrientationFromQuaternion)(const Quaternion&) = &PipelineCamera::SetOrientation;
	class_<PipelineCamera, boost::noncopyable>("PipelineCamera", no_init)
		.def("SetLocation", SetLocationFromLocation)
		.def("SetOrientation", SetOrientationFromQuaternion)
		.def("SetPose", &PipelineCamera::SetPose)
		.def("BuildMatrices", &PipelineCamera::BuildMatrices);

	StlInterface();
	EntitySpecificationInterface();
	GraphicsObjectInterface(scene_registration);
	IOInterfaceInterface(scene_registration);
	HeadsetInterface(scene_registration);
	GraphicsResourcesInterface();
	PoseInterface();
	ModelCreationInterface();
	GrabbableObjectHandlerInterface(scene_registration);
	CollisionShapeInterface();
	LightInterface();
	Physics(scene_registration);
	Bullet(scene_registration);
}