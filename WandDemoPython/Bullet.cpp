#include "stdafx.h"
#include "Physics.h"
#include "StlHelper.h"

#include "SceneSystem/BulletInternals.h"

using bullet::World;
using bullet::Shape;
using bullet::CollisionObject;
using bullet::RigidBody;
using bullet::poses::GetTransform;
using bullet::poses::GetVector3;
using bullet::poses::GetPose;
using bullet::CollisionResult;

std::auto_ptr<RigidBody> MakeRigidBody(std::auto_ptr<Shape> shape_ptr, Pose transform) {
	Shape shape(std::move(*shape_ptr));
	auto rigid_body = std::make_unique<RigidBody>(std::move(shape), GetTransform(transform));
	return std::auto_ptr<RigidBody>(rigid_body.release());
}

std::auto_ptr<RigidBody> MakeRigidBody(std::auto_ptr<Shape> shape_ptr, Pose transform, RigidBody::InteractionType interaction_type) {
	Shape shape(std::move(*shape_ptr));
	auto rigid_body = std::make_unique<RigidBody>(std::move(shape), GetTransform(transform), interaction_type);
	return std::auto_ptr<RigidBody>(rigid_body.release());
}

std::auto_ptr<CollisionObject> MakeCollisionObject(
  uint8_t collision_object_type, std::auto_ptr<Shape> shape_ptr, Pose transform) {
  Shape shape(std::move(*shape_ptr));
  auto collision_object = std::make_unique<CollisionObject>(static_cast<CollisionObject::CollisionObjectType>(collision_object_type), std::move(shape), GetTransform(transform));
  return std::auto_ptr<CollisionObject>(collision_object.release());
}

std::auto_ptr<Shape> MakeAutoSphere(btScalar radius) {
	return std::auto_ptr<Shape>(new Shape(std::move(Shape::MakeSphere(radius))));
}

std::auto_ptr<Shape> MakeAutoPlane(Location normal) {
	return std::auto_ptr<Shape>(new Shape(std::move(Shape::MakePlane(GetVector3(normal)))));
}

std::auto_ptr<Shape> MakeAutoPlaneConstant(Location normal, btScalar plane_constant) {
	return std::auto_ptr<Shape>(new Shape(std::move(Shape::MakePlane(GetVector3(normal), plane_constant))));
}

std::auto_ptr<Shape> MakeAutoPlanePoint(Location normal, Location point_in_plane) {
	return std::auto_ptr<Shape>(new Shape(std::move(Shape::MakePlane(GetVector3(normal), GetVector3(point_in_plane)))));
}

void Bullet(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<Shape, std::auto_ptr<Shape>, boost::noncopyable>("Shape", no_init)
		.def("MakeSphere", MakeAutoSphere)
		.staticmethod("MakeSphere")
		.def("MakePlane", MakeAutoPlane)
		.def("MakePlane", MakeAutoPlaneConstant)
		.def("MakePlane", MakeAutoPlanePoint)
		.staticmethod("MakePlane");

  class_<CollisionResult, std::auto_ptr<CollisionResult>, boost::noncopyable>("CollisionResult", no_init)
    .def("CollisionFound", &CollisionResult::CollisionFound)
    .def("GetCollidedObjects", &CollisionResult::GetExistingWrappedObjects);

  class_<World, boost::noncopyable>("World", no_init)
    .def("AddRigidBody", static_cast<void(World::*)(const RigidBody&)>(&World::AddRigidBody))
    .def("RemoveRigidBody", static_cast<void(World::*)(const RigidBody&)>(&World::RemoveRigidBody))
    .def("AddCollisionObject", static_cast<void(World::*)(const CollisionObject&)>(&World::AddCollisionObject))
    .def("RemoveCollisionObject", static_cast<void(World::*)(const CollisionObject&)>(&World::RemoveCollisionObject))
    .def("CheckCollision", 
      static_cast<CollisionResult*(*)(World&, const CollisionObject&)>([](World& world, const CollisionObject& check_object) -> CollisionResult* {
        return new CollisionResult(std::move(world.CheckCollision(check_object)));
      }),
      return_value_policy<boost::python::manage_new_object>());

  class_<RigidBody::InteractionType>("InteractionType", init<>())
    .def(init<btScalar>())
    .def(init<btScalar, btVector3>())
    .def("Kinematic", &RigidBody::InteractionType::Kinematic)
    .staticmethod("Kinematic")
    .def("Static", &RigidBody::InteractionType::Static)
    .staticmethod("Static");

  enum_<CollisionObject::CollisionObjectType>("CollisionObjectType")
    .value("NORMAL", CollisionObject::CollisionObjectType::NORMAL)
    .value("GHOST", CollisionObject::CollisionObjectType::GHOST)
    .value("PAIR_CACHING_GHOST", CollisionObject::CollisionObjectType::PAIR_CACHING_GHOST);

  class_<CollisionObject, std::auto_ptr<CollisionObject>, boost::noncopyable>("CollisionObject", no_init)
    .def("__init__", boost::python::make_constructor(
      static_cast<std::auto_ptr<CollisionObject>(*)(uint8_t, std::auto_ptr<Shape>, Pose)>(MakeCollisionObject)))
    .def("GetPose", static_cast<Pose(*)(CollisionObject&)>([](CollisionObject& collision_object) -> Pose {return GetPose(collision_object.GetTransform()); }))
    .def("SetPose", static_cast<void(*)(CollisionObject&, Pose)>([](CollisionObject& collision_object, Pose pose) -> void {return collision_object.SetTransform(GetTransform(pose)); }))
    .def("SetShape", static_cast<void(*)(CollisionObject&, std::auto_ptr<Shape>)>(
      [](CollisionObject& collision_object, std::auto_ptr<Shape> shape) -> void {
        collision_object.SetShape(std::move(*shape));
      }))
    .def("GetShape", &CollisionObject::GetShape, return_value_policy<reference_existing_object>());

  class_<RigidBody, bases<CollisionObject>, std::auto_ptr<RigidBody>, boost::noncopyable>("RigidBody", no_init)
    .def("__init__", boost::python::make_constructor(
      static_cast<std::auto_ptr<RigidBody>(*)(std::auto_ptr<Shape>, Pose)>(MakeRigidBody)))
    .def("__init__", boost::python::make_constructor(
      static_cast<std::auto_ptr<RigidBody>(*)(std::auto_ptr<Shape>, Pose, RigidBody::InteractionType)>(MakeRigidBody)));

  CreateVector<CollisionObject*>("CollisionObjectPtr");
}
