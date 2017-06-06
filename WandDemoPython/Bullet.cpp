#include "stdafx.h"
#include "Physics.h"
#include "StlHelper.h"

#include "SceneSystem/BulletInternals.h"

std::auto_ptr<bullet::RigidBody> MakeRigidBody(std::auto_ptr<bullet::Shape> shape_ptr, Pose transform) {
	bullet::Shape shape(std::move(*shape_ptr));
	auto rigid_body = std::make_unique<bullet::RigidBody>(std::move(shape), bullet::poses::GetTransform(transform));
	return std::auto_ptr<bullet::RigidBody>(rigid_body.release());
}

std::auto_ptr<bullet::RigidBody> MakeRigidBody(std::auto_ptr<bullet::Shape> shape_ptr, Pose transform, bullet::RigidBody::InteractionType interaction_type) {
	bullet::Shape shape(std::move(*shape_ptr));
	auto rigid_body = std::make_unique<bullet::RigidBody>(std::move(shape), bullet::poses::GetTransform(transform), interaction_type);
	return std::auto_ptr<bullet::RigidBody>(rigid_body.release());
}

std::auto_ptr<bullet::CollisionObject> MakeCollisionObject(
  bullet::CollisionObject::CollisionObjectType collision_object_type, std::auto_ptr<bullet::Shape> shape_ptr, Pose transform) {
  bullet::Shape shape(std::move(*shape_ptr));
  auto collision_object = std::make_unique<bullet::CollisionObject>(collision_object_type, std::move(shape), bullet::poses::GetTransform(transform));
  return std::auto_ptr<bullet::CollisionObject>(collision_object.release());
}

std::auto_ptr<bullet::Shape> MakeAutoSphere(btScalar radius) {
	return std::auto_ptr<bullet::Shape>(new bullet::Shape(std::move(bullet::Shape::MakeSphere(radius))));
}

std::auto_ptr<bullet::Shape> MakeAutoPlane(Location normal) {
	return std::auto_ptr<bullet::Shape>(new bullet::Shape(std::move(bullet::Shape::MakePlane(bullet::poses::GetVector3(normal)))));
}

std::auto_ptr<bullet::Shape> MakeAutoPlaneConstant(Location normal, btScalar plane_constant) {
	return std::auto_ptr<bullet::Shape>(new bullet::Shape(std::move(bullet::Shape::MakePlane(bullet::poses::GetVector3(normal), plane_constant))));
}

std::auto_ptr<bullet::Shape> MakeAutoPlanePoint(Location normal, Location point_in_plane) {
	return std::auto_ptr<bullet::Shape>(new bullet::Shape(std::move(bullet::Shape::MakePlane(bullet::poses::GetVector3(normal), bullet::poses::GetVector3(point_in_plane)))));
}

void Bullet(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<bullet::Shape, std::auto_ptr<bullet::Shape>, boost::noncopyable>("Shape", no_init)
		.def("MakeSphere", MakeAutoSphere)
		.staticmethod("MakeSphere")
		.def("MakePlane", MakeAutoPlane)
		.def("MakePlane", MakeAutoPlaneConstant)
		.def("MakePlane", MakeAutoPlanePoint)
		.staticmethod("MakePlane");

  class_<bullet::RigidBody::InteractionType>("InteractionType", init<>())
    .def(init<btScalar>())
    .def(init<btScalar, btVector3>())
    .def("Kinematic", &bullet::RigidBody::InteractionType::Kinematic)
    .staticmethod("Kinematic")
    .def("Static", &bullet::RigidBody::InteractionType::Static)
    .staticmethod("Static");

	class_<bullet::RigidBody, std::auto_ptr<bullet::RigidBody>, boost::noncopyable>("RigidBody", no_init)
		.def("__init__", boost::python::make_constructor(
			static_cast<std::auto_ptr<bullet::RigidBody>(*)(std::auto_ptr<bullet::Shape>, Pose)>(MakeRigidBody)))
		.def("__init__", boost::python::make_constructor(
			static_cast<std::auto_ptr<bullet::RigidBody>(*)(std::auto_ptr<bullet::Shape>, Pose, bullet::RigidBody::InteractionType)>(MakeRigidBody)))
    .def("GetPose", (Pose(*)(bullet::RigidBody&))+[](bullet::RigidBody& rigid_body) {return bullet::poses::GetPose(rigid_body.GetTransform()); })
    .def("SetPose", (Pose(*)(bullet::RigidBody&, Pose))+[](bullet::RigidBody& rigid_body, Pose pose) {return rigid_body.SetTransform(bullet::poses::GetTransform(pose)); });

  class_<bullet::CollisionObject, std::auto_ptr<bullet::CollisionObject>, boost::noncopyable>("CollisionObject", no_init)
    .def("__init__", boost::python::make_constructor(
      static_cast<std::auto_ptr<bullet::CollisionObject>(*)(bullet::CollisionObject::CollisionObjectType, std::auto_ptr<bullet::Shape>, Pose)>(MakeCollisionObject)))
    .add_static_property("NORMAL", bullet::CollisionObject::CollisionObjectType::NORMAL)
    .add_static_property("GHOST", bullet::CollisionObject::CollisionObjectType::GHOST)
    .add_static_property("PAIR_CACHING_GHOST", bullet::CollisionObject::CollisionObjectType::PAIR_CACHING_GHOST)
    .def("GetPose", (Pose(*)(bullet::CollisionObject&))+[](bullet::CollisionObject& collision_object) {return bullet::poses::GetPose(collision_object.GetTransform()); })
    .def("SetPose", (Pose(*)(bullet::CollisionObject&, Pose))+[](bullet::CollisionObject& collision_object, Pose pose) {return collision_object.SetTransform(bullet::poses::GetTransform(pose)); });
}
