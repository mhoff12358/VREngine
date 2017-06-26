#include "stdafx.h"
#include "Physics.h"

#include "PyScene.h"
#include "PyCommandAndQuery.h"
#include "SceneSystem/PhysicsObject.h"
#include "SceneSystem/PhysicsSimulation.h"

BOOST_PTR_MAGIC_STRUCT(game_scene::AddRigidBody)
BOOST_PTR_MAGIC_STRUCT(game_scene::RemoveRigidBody)
BOOST_PTR_MAGIC(game_scene::UpdatePhysicsObject)

/*std::auto_ptr<game_scene::AddRigidBody> MakeAddRigidBody(
	string name, PxRigidActor* rigid_body_ptr) {
	PxRigidActor* rigid_body = std::move(*rigid_body_ptr);
	auto add_rigid_body = std::make_unique<game_scene::AddRigidBody>(name, std::move(rigid_body));
	return std::auto_ptr<game_scene::AddRigidBody>(add_rigid_body.release());
}*/

void Physics(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<game_scene::PhysicsObjectCommand>("PhysicsObjectCommand")
		.def_readonly("ADD_RIGID_BODY", &game_scene::PhysicsObjectCommand::ADD_RIGID_BODY)
		.def_readonly("REMOVE_RIGID_BODY", &game_scene::PhysicsObjectCommand::REMOVE_RIGID_BODY)
		.def_readonly("ADD_UPDATED_CALLBACK", &game_scene::PhysicsObjectCommand::ADD_UPDATED_CALLBACK);

	class_<game_scene::PhysicsObjectQuery>("PhysicsObjectQuery")
		.def_readonly("GET_RIGID_BODIES", &game_scene::PhysicsObjectQuery::GET_RIGID_BODIES)
		.def_readonly("CHECK_COLLISION", &game_scene::PhysicsObjectQuery::CHECK_COLLISION);

  /*class_<game_scene::CheckCollisionQuery, bases<game_scene::QueryArgs>, boost::noncopyable>("CheckCollisionQuery", init<bullet::World&, bullet::CollisionObject&>());
  class_<game_scene::CheckCollisionResult, bases<game_scene::QueryResult>, boost::noncopyable>("CheckCollisionResult", no_init)
    .def_readonly("collision", &game_scene::CheckCollisionResult::collision_);*/

	/*class_<game_scene::AddRigidBody, bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::AddRigidBody>,
		boost::noncopyable>("AddRigidBody", init<string, bool, PxRigidActor*>())
		.def(init<string, PxRigidActor*>())
		.def(init<bool, PxRigidActor*>())
		.def(init<PxRigidActor*>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::AddRigidBody>);

	class_<game_scene::RemoveRigidBody, bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::RemoveRigidBody>,
		boost::noncopyable>("RemoveRigidBody", init<string>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::RemoveRigidBody>);

	class_<game_scene::PhysicsSimulationCommand>("PhysicsSimulationCommand")
		.def_readonly("UPDATE_PHYSICS_OBJECT", &game_scene::PhysicsSimulationCommand::UPDATE_PHYSICS_OBJECT)
		.def_readonly("GET_SCENE", &game_scene::PhysicsSimulationCommand::GET_SCENE);
  RegisterQueryResultWrapped<bullet::World&>("World");

	enum_<game_scene::UpdatePhysicsObject::UpdateType>("UpdateType")
		.value("ADD", game_scene::UpdatePhysicsObject::UpdateType::ADD)
		.value("REMOVE", game_scene::UpdatePhysicsObject::UpdateType::REMOVE);
	class_<game_scene::UpdatePhysicsObject, bases<game_scene::CommandArgs>,
		std::auto_ptr<game_scene::UpdatePhysicsObject>,
		boost::noncopyable>(
			"UpdatePhysicsObject",
			init<game_scene::UpdatePhysicsObject::UpdateType, game_scene::ActorId>());
	scene_registration.def(
		"MakeCommandAfter",
		&PyScene::MakeCommandAfter<game_scene::UpdatePhysicsObject>);*/
}
