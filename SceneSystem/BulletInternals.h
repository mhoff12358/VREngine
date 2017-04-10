#pragma once

#include "stdafx.h"

#include "VRBackend/Pose.h"
#include <btBulletDynamicsCommon.h>

#include "ActorId.h"

namespace bullet {

class Config {
public:
	Config();

	unique_ptr<btBroadphaseInterface> broadphase_;

	unique_ptr<btDefaultCollisionConfiguration> collision_config_;
	unique_ptr<btCollisionDispatcher> collision_dispatch_;

	unique_ptr<btSequentialImpulseConstraintSolver> solver_;
};

using Shape = unique_ptr<btCollisionShape>;
namespace shapes {
	Shape MakeSphere(btScalar radius);
	Shape MakePlane(btVector3 normal, btScalar plane_constant = 0);
	Shape MakePlane(btVector3 normal, btVector3 point_in_plane);
}  // shapes

struct CollisionComponent {
	game_scene::ActorId actor_ = game_scene::ActorId::UnsetId;
	unsigned int rigid_body_ident_ = 0;

	bool operator<(const CollisionComponent other) const;
	bool operator==(const CollisionComponent other) const;
};

struct RigidBodyPayload {
	CollisionComponent collision_component_;
};

using CollisionInfo = pair<
	bullet::CollisionComponent,
	tuple<
		bullet::CollisionComponent,
		const btCollisionObjectWrapper*,
		const btCollisionObjectWrapper*>>;
namespace collision_info {
	CollisionInfo Make(
		bullet::CollisionComponent cc_a, const btCollisionObjectWrapper* wrapper_a,
		bullet::CollisionComponent cc_b, const btCollisionObjectWrapper* wrapper_b);
}  // collision_info


class RigidBody {
public:
	RigidBody(
		Shape shape,
		unique_ptr<btDefaultMotionState> starting_motion_state,
		btScalar mass = 1,
		btVector3 inertia = btVector3(0, 0, 0));
	RigidBody(
		Shape shape,
		btTransform transform,
		btScalar mass = 1,
		btVector3 inertia = btVector3(0, 0, 0));

	btRigidBody* GetBody() const;

	const RigidBodyPayload& GetPayload() const;
	void SetPayload(RigidBodyPayload* payload);

	const btDefaultMotionState& GetMotionState() const;
	btDefaultMotionState* GetMutableMotionState();
	btTransform GetTransform() const;
	void SetTransform(btTransform new_transform);
 
	void MakeCollideable(bool collideable = true) const;
	bool IsCollideable() const;

private:
	unique_ptr<btRigidBody> body_;
	Shape shape_;
	unique_ptr<btDefaultMotionState> motion_state_;
};

class World {
public:
	World(Config config);

	void Step(btScalar time_step);

	void AddRigidBody(const RigidBody& body);
	void AddRigidBody(btRigidBody* body);
	void RemoveRigidBody(const RigidBody& body);
	void RemoveRigidBody(btRigidBody* body);

private:
	Config config_;
	unique_ptr<btDiscreteDynamicsWorld> world_;
	unsigned int substep_limit_ = 10;
};

namespace poses {
	btQuaternion GetQuaternion(const Quaternion& quat);
	btVector3 GetVector3(const Location& loc);
	btTransform GetTransform(const Pose& pose);
	Quaternion GetQuaternion(const btQuaternion& quat);
	Location GetLocation(const btVector3& loc);
	Pose GetPose(const btTransform& pose);
}  // poses
}  // bullet

