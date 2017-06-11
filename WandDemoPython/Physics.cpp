#include "stdafx.h"
#include "Physics.h"

#include "PyScene.h"
#include "PyCommandAndQuery.h"
#include "SceneSystem/PhysicsObject.h"
#include "SceneSystem/PhysicsSimulation.h"

BOOST_PTR_MAGIC_STRUCT(game_scene::commands::AddRigidBody)
BOOST_PTR_MAGIC_STRUCT(game_scene::commands::RemoveRigidBody)
BOOST_PTR_MAGIC(game_scene::commands::UpdatePhysicsObject)

std::auto_ptr<game_scene::commands::AddRigidBody> MakeAddRigidBody(
	string name, std::auto_ptr<bullet::RigidBody> rigid_body_ptr) {
	bullet::RigidBody rigid_body = std::move(*rigid_body_ptr);
	auto add_rigid_body = std::make_unique<game_scene::commands::AddRigidBody>(name, std::move(rigid_body));
	return std::auto_ptr<game_scene::commands::AddRigidBody>(add_rigid_body.release());
}

game_scene::commands::ModifyRigidBody::ModifyFunction MakeModifierFunction(object callback) {
  return [callback](bullet::RigidBody& body) {
    return static_cast<bool>(boost::python::extract<bool>(callback(body)));
  };
}

void Physics(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<game_scene::commands::PhysicsObjectCommand>("PhysicsObjectCommand")
		.def_readonly("ADD_RIGID_BODY", &game_scene::commands::PhysicsObjectCommand::ADD_RIGID_BODY)
		.def_readonly("REMOVE_RIGID_BODY", &game_scene::commands::PhysicsObjectCommand::REMOVE_RIGID_BODY)
		.def_readonly("MODIFY_RIGID_BODY", &game_scene::commands::PhysicsObjectCommand::MODIFY_RIGID_BODY)
		.def_readonly("ADD_UPDATED_CALLBACK", &game_scene::commands::PhysicsObjectCommand::ADD_UPDATED_CALLBACK);

	class_<game_scene::queries::PhysicsObjectQuery>("PhysicsObjectQuery")
		.def_readonly("GET_RIGID_BODIES", &game_scene::queries::PhysicsObjectQuery::GET_RIGID_BODIES)
		.def_readonly("CHECK_COLLISION", &game_scene::queries::PhysicsObjectQuery::CHECK_COLLISION);

  class_<game_scene::queries::CheckCollisionQuery, bases<game_scene::QueryArgs>, boost::noncopyable>("CheckCollisionQuery", init<bullet::World&, bullet::CollisionObject&>());
  class_<game_scene::queries::CheckCollisionResult, bases<game_scene::QueryResult>, boost::noncopyable>("CheckCollisionResult", no_init)
    .def_readonly("collision", &game_scene::queries::CheckCollisionResult::collision_);

	class_<game_scene::commands::AddRigidBody, bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::commands::AddRigidBody>,
		boost::noncopyable>("AddRigidBody", no_init)
		.def("__init__", boost::python::make_constructor(MakeAddRigidBody));
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::commands::AddRigidBody>);

	class_<game_scene::commands::RemoveRigidBody, bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::commands::RemoveRigidBody>,
		boost::noncopyable>("RemoveRigidBody", init<string>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::commands::RemoveRigidBody>);

	class_<game_scene::commands::PhysicsSimulationCommand>("PhysicsSimulationCommand")
		.def_readonly("UPDATE_PHYSICS_OBJECT", &game_scene::commands::PhysicsSimulationCommand::UPDATE_PHYSICS_OBJECT)
		.def_readonly("GET_WORLD", &game_scene::commands::PhysicsSimulationCommand::GET_WORLD);
  RegisterQueryResultWrapped<bullet::World&>("World");

	enum_<game_scene::commands::UpdatePhysicsObject::UpdateType>("UpdateType")
		.value("ADD", game_scene::commands::UpdatePhysicsObject::UpdateType::ADD)
		.value("REMOVE", game_scene::commands::UpdatePhysicsObject::UpdateType::REMOVE);
	class_<game_scene::commands::UpdatePhysicsObject, bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::commands::UpdatePhysicsObject>,
		boost::noncopyable>(
			"UpdatePhysicsObject",
			init<game_scene::commands::UpdatePhysicsObject::UpdateType, game_scene::ActorId>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::commands::UpdatePhysicsObject>);
}
