#include "stdafx.h"
#include "Physics.h"
#include "StlHelper.h"
#include "MyPhysics.h"

#include "SceneSystem/PhysXInternal.h"


void PhysX(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<PxGeometry, boost::noncopyable>("Geometry", no_init);
	class_<PxBoxGeometry, bases<PxGeometry>, boost::noncopyable>("BoxGeometry", init<float, float, float>());

	class_<::mp::PxActor, boost::noncopyable>("PxActor", no_init);
	class_<::mp::PxRigidActor, bases<::mp::PxActor>, boost::noncopyable>("PxRigidActor", no_init);

	class_<::mp::PxScene, boost::noncopyable>("PxScene", no_init)
		.def("Hello", static_cast<float(*)(mp::PxScene&)>([](mp::PxScene& w) { return mp::Unwrap(w).getClothInterCollisionDistance(); }))
		.def("Hi", static_cast<void(*)(mp::PxScene&, float f)>([](mp::PxScene& w, float f) { mp::Unwrap(w).setClothInterCollisionDistance(f); }));

	/*class_<PxActor, boost::noncopyable>("PxActor", no_init);
	class_<PxRigidActor, bases<PxActor>, boost::noncopyable>("PxRigidActor", no_init);
	class_<PxRigidBody, bases<PxRigidActor>, boost::noncopyable>("PxRigidBody", no_init);
	class_<PxRigidDynamic, bases<PxRigidBody>, boost::noncopyable>("PxRigidDynamic", no_init);
	class_<PxRigidStatic, bases<PxRigidActor>, boost::noncopyable>("PxRigidStatic", no_init);*/

}