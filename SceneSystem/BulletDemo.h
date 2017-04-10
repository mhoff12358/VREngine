#pragma once

#include "stdafx.h"

#include "VRBackend/Pose.h"
#include <btBulletDynamicsCommon.h>

#include "Actor.h"
#include "InputCommandArgs.h"

namespace game_scene {
namespace actors {

template<typename ActorBase>
class PhysicsObject : public ActorBase {
public:
	void HandleCommand(const CommandArgs& args) {
		switch (args.Type()) {
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	string GetName() const {
		return "PhysicsObject-" + ActorBase::GetName();
	}
};

}  // actor
}  // game_scene

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

namespace game_scene {

class PhysicsSimulationCommand : public CommandType {
public:
	DECLARE_COMMAND(PhysicsSimulationCommand, ADD_RIGID_BODY);
	DECLARE_COMMAND(PhysicsSimulationCommand, REMOVE_RIGID_BODY);
};

class AddRemoveRigidBody : public CommandArgs {
public:
	AddRemoveRigidBody(bullet::RigidBody& rigid_body, bool add_not_remove = true) :
		CommandArgs(add_not_remove ? PhysicsSimulationCommand::ADD_RIGID_BODY : PhysicsSimulationCommand::REMOVE_RIGID_BODY),
		rigid_body_(rigid_body) {}

	bullet::RigidBody& rigid_body_;
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

class PhysicsSimulationImpl {

public:
	PhysicsSimulationImpl(bullet::World world);

	void HandleTimeTick(const commands::TimeTick& args);

	void MakeCallbackOwner();

	void PreTimeTick();
	void PostTimeTick();

private:
	bullet::World world_;

	map<CollisionInfo::first_type, CollisionInfo::second_type> current_collisions_;

	static PhysicsSimulationImpl* callback_owner;
	bool ContactProcessedCallback(
		btManifoldPoint& cp,
		void* body0, void* body1);

	bool ContactAddedCallback(
		btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1, int index1);

	static bool ContactProcessedCallbackDelegater(
		btManifoldPoint& cp,
		void* body0, void* body1);

	static bool ContactAddedCallbackDelegater(
		btManifoldPoint& cp,
		const btCollisionObjectWrapper* colObj0Wrap,
		int partId0, int index0,
		const btCollisionObjectWrapper* colObj1Wrap,
		int partId1, int index1);
};

template<typename ActorBase>
class PhysicsSimulation : public ActorBase {
public:
	void HandleCommand(const CommandArgs& args) {
		switch (args.Type()) {
		case InputCommand::TICK:
			physics_simulation_impl_.HandleTimeTick(dynamic_cast<const commands::TimeTick&>(args));
			break;
		default:
			break;
		}
		ActorBase::HandleCommand(args);

public:
	PhysicsSimulationImpl physics_simulation_impl_;
};
};
}  // game_scene
*/