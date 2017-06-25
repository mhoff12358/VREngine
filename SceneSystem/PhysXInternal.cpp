#include "stdafx.h"

#include "PhysXInternal.h"

PhysicsSystem::PhysicsSystem() {
	foundation_ = PxCreateFoundation(PX_FOUNDATION_VERSION, allocator_, error_callback_);
	physics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation_, PxTolerancesScale(), true);
	dispatcher_ = PxDefaultCpuDispatcherCreate(2);

	default_material_ = physics_->createMaterial(0.5f, 0.5f, 0.1f);
}

PhysicsScene::PhysicsScene(const PhysicsSystem& system) : system_(system) {
	PxSceneDesc desc(system_.physics_->getTolerancesScale());
	desc.flags.set(PxSceneFlag::eENABLE_ACTIVE_ACTORS);
	desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
	desc.cpuDispatcher = system_.dispatcher_;
	desc.filterShader = PxDefaultSimulationFilterShader;
	scene_ = system_.physics_->createScene(desc);
}

void PhysicsScene::Step() {
	scene_->simulate(1.0f / 60.0f);
	scene_->fetchResults(true);
}

void PhysicsScene::DummyPhysics() {
	PxRigidDynamic* box = system_.physics_->createRigidDynamic(poses::ToTransform(Pose(Location(0, 3, 0), Quaternion::RotationAboutAxis(AID_X, 3.14f/3))));
	PxRigidActorExt::createExclusiveShape(*box, PxBoxGeometry(1, 1, 1), &system_.default_material_, 1);
	PxRigidBodyExt::updateMassAndInertia(*box, 10.0f);

	PxRigidStatic* floor = PxCreatePlane(*system_.physics_, PxPlane(0, 1, 0, 0), *system_.default_material_);

	scene_->addActor(*box);
	scene_->addActor(*floor);

	while (true) {
		Step();

		PxU32 num_actors;
		PxActor** active_actors = scene_->getActiveActors(num_actors);
		for (PxU32 i = 0; i < num_actors; i++) {
			PxRigidActor* actor = active_actors[i]->is<PxRigidActor>();
			if (actor) {
				std::cout << "New pose: " << std::to_string(poses::ToPose(actor->getGlobalPose())) << std::endl;
			}
			else {
				std::cout << "NON RIGID ACTOR" << std::endl;
			}
		}
	}
}

namespace poses {
	PxTransform ToTransform(const Pose& pose) {
		return PxTransform(ToVec3(pose.location_), ToQuat(pose.orientation_));
	}
	Pose ToPose(const PxTransform& transform) {
		return Pose(ToLocation(transform.p), ToQuaternion(transform.q));
	}
	PxVec3 ToVec3(const Location& location) {
		return PxVec3(location[0], location[1], location[1]);
	}
	Location ToLocation(const PxVec3& vec3) {
		return Location(vec3[0], vec3[1], vec3[2]);
	}
	PxQuat ToQuat(const Quaternion& quat) {
		return PxQuat(quat.x, quat.y, quat.z, quat.w);
	}
	Quaternion ToQuaternion(const PxQuat& quaternion) {
		return Quaternion(quaternion.x, quaternion.y, quaternion.z, quaternion.w);
	}
}
