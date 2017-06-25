#pragma once

#include "stdafx.h"

#include "VRBackend/Pose.h"
#include "PxPhysicsAPI.h"

using namespace physx;

class PhysicsSystem {
public:

	PhysicsSystem();

	PxPhysics* physics_ = nullptr;
	PxFoundation* foundation_ = nullptr;

	PxDefaultAllocator allocator_;
	PxDefaultErrorCallback error_callback_;
	PxDefaultCpuDispatcher* dispatcher_;

	PxMaterial* default_material_;
};

class PhysicsScene {
public:
	PhysicsScene(const PhysicsSystem& system);

	void Step();

	void DummyPhysics();

	PxScene* scene_;
	const PhysicsSystem& system_;
};

namespace poses {
	PxTransform ToTransform(const Pose& pose);
	Pose ToPose(const PxTransform& pose);
	PxVec3 ToVec3(const Location& pose);
	Location ToLocation(const PxVec3& pose);
	PxQuat ToQuat(const Quaternion& pose);
	Quaternion ToQuaternion(const PxQuat& pose);
}

