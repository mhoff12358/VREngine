#include "stdafx.h"
#include "Physics.h"
#include "StlHelper.h"

#include "SceneSystem/PhysXInternal.h"

void PhysX(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<PxGeometry, boost::noncopyable>("Geometry", no_init);
	class_<PxBoxGeometry, bases<PxGeometry>, boost::noncopyable>("BoxGeometry", init<float, float, float>());

	class_<PxActor, boost::noncopyable>("PxActor", no_init);
	class_<PxRigidActor, bases<PxActor>, boost::noncopyable>("PxRigidActor", no_init);
	class_<PxRigidBody, bases<PxRigidActor>, boost::noncopyable>("PxRigidBody", no_init);
	class_<PxRigidDynamic, bases<PxRigidBody>, boost::noncopyable>("PxRigidDynamic", no_init);
	class_<PxRigidStatic, bases<PxRigidActor>, boost::noncopyable>("PxRigidStatic", no_init);

}