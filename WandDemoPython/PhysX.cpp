#include "stdafx.h"
#include "Physics.h"
#include "StlHelper.h"
#include "MyPhysics.h"

#include "SceneSystem/PhysXInternal.h"


void PhysX(class_<game_scene::Scene, boost::noncopyable>& scene_registration) {
	class_<PxVec3>("PxVec3", init<float, float, float>());
	class_<PxQuat>("PxQuat", init<float, float, float, float>());
	class_<PxTransform>("PxTransform", init<PxVec3, PxQuat>());

	class_<PxGeometry, boost::noncopyable>("PxGeometry", no_init);
	class_<PxBoxGeometry, bases<PxGeometry>, boost::noncopyable>("PxBoxGeometry", init<float, float, float>());
	class_<PxPlane>("PxPlane", init<float, float, float, float>());

	class_<mp::PxActor, boost::noncopyable>("PxActor", no_init);
	class_<mp::PxRigidActor, bases<mp::PxActor>, boost::noncopyable>("PxRigidActor", no_init)
		.def("CreateExclusiveShape", static_cast<void(*)(mp::PxRigidActor& actor, PxBoxGeometry, const mp::PxMaterial&)>(
			[](mp::PxRigidActor& actor, PxBoxGeometry box, const mp::PxMaterial& material) {
		PxRigidActorExt::createExclusiveShape(mp::Unwrap(actor), box, mp::Unwrap(material));
	}));
	class_<mp::PxRigidBody, bases<mp::PxRigidActor>, boost::noncopyable>("PxRigidBody", no_init)
		.def("UpdateMassAndInertia", static_cast<void(*)(mp::PxRigidBody& body, float)>(
			[](mp::PxRigidBody& body, float density) {
		PxRigidBodyExt::updateMassAndInertia(mp::Unwrap(body), density);
	}));
	class_<mp::PxRigidDynamic, bases<mp::PxRigidBody>, boost::noncopyable>("PxRigidDynamic", no_init);
	class_<mp::PxRigidStatic, bases<mp::PxRigidActor>, boost::noncopyable>("PxRigidStatic", no_init);

	class_<mp::PxScene, boost::noncopyable>("PxScene", no_init)
		.def("AddActor", static_cast<void(*)(mp::PxScene&, mp::PxActor&)>([](mp::PxScene& scene, mp::PxActor& actor) {
		mp::Unwrap(scene).addActor(mp::Unwrap(actor));
	}))
		.def("Hello", static_cast<float(*)(mp::PxScene&)>([](mp::PxScene& w) { return mp::Unwrap(w).getClothInterCollisionDistance(); }))
		.def("Hi", static_cast<void(*)(mp::PxScene&, float f)>([](mp::PxScene& w, float f) { mp::Unwrap(w).setClothInterCollisionDistance(f); }));

	class_<mp::PxPhysics, boost::noncopyable>("PxPhysics", no_init)
		.def("CreateRigidDynamic", static_cast<mp::PxRigidDynamic*(*)(mp::PxPhysics&, const PxTransform&)>(
			[](mp::PxPhysics& phys, const PxTransform& trans) {
		return mp::Wrap(mp::Unwrap(phys).createRigidDynamic(trans));
	}), return_value_policy<reference_existing_object>())
		.def("CreatePlane", static_cast<mp::PxRigidStatic*(*)(mp::PxPhysics&, PxPlane, mp::PxMaterial&)>(
			[](mp::PxPhysics& phys, PxPlane plane, mp::PxMaterial& material) {
		return mp::Wrap(PxCreatePlane(mp::Unwrap(phys), plane, mp::Unwrap(material)));
	}), return_value_policy<reference_existing_object>());

}