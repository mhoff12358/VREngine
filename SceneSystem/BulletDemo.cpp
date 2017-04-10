#include "stdafx.h"
#include "BulletDemo.h"
#include "InputCommandArgs.h"

namespace bullet {

Config::Config() :
	broadphase_(new btDbvtBroadphase()),
	collision_config_(new btDefaultCollisionConfiguration()),
	collision_dispatch_(new btCollisionDispatcher(collision_config_.get())),
	solver_(new btSequentialImpulseConstraintSolver)
{
}

bool CollisionComponent::operator<(const CollisionComponent other) const {
	if (actor_ < other.actor_) {
		return true;
	} else if (other.actor_ < actor_) {
		return false;
	} else {
		return rigid_body_ident_ < other.rigid_body_ident_;
	}
}

bool CollisionComponent::operator==(const CollisionComponent other) const {
	return (actor_ == other.actor_) && (rigid_body_ident_ == other.rigid_body_ident_);
}

RigidBody::RigidBody(
	Shape shape,
	btTransform transform,
	btScalar mass,
	btVector3 inertia) :
	RigidBody(std::move(shape), make_unique<btDefaultMotionState>(transform), mass, inertia) {}

RigidBody::RigidBody(
	Shape shape,
	unique_ptr<btDefaultMotionState> starting_motion_state,
	btScalar mass,
	btVector3 inertia) :
	shape_(std::move(shape)),
	motion_state_(std::move(starting_motion_state))
{
	shape_->calculateLocalInertia(mass, inertia);
    btRigidBody::btRigidBodyConstructionInfo construction_info(mass, motion_state_.get(), shape_.get(), inertia);
	body_ = make_unique<btRigidBody>(construction_info);
}

btRigidBody* RigidBody::GetBody() const {
	return body_.get();
}

const RigidBodyPayload& RigidBody::GetPayload() const {
	return *static_cast<RigidBodyPayload*>((shape_->getUserPointer()));
}

void RigidBody::SetPayload(RigidBodyPayload* payload) {
	shape_->setUserPointer(static_cast<void*>(payload));
}

World::World(Config config) :
	config_(std::move(config)),
	world_(new btDiscreteDynamicsWorld(
		config_.collision_dispatch_.get(),
		config_.broadphase_.get(),
		config_.solver_.get(),
		config_.collision_config_.get())) {
	world_->setGravity(btVector3(0, -10, 0));
}

void World::Step(btScalar time_step) {
	world_->stepSimulation(time_step, substep_limit_);
}

void World::AddRigidBody(const RigidBody& body) {
	AddRigidBody(body.GetBody());
}
void World::AddRigidBody(btRigidBody* body) {
	world_->addRigidBody(body);
}

void World::RemoveRigidBody(const RigidBody& body) {
	RemoveRigidBody(body.GetBody());
}
void World::RemoveRigidBody(btRigidBody* body) {
	world_->removeRigidBody(body);
}

const btDefaultMotionState& RigidBody::GetMotionState() const {
	return *motion_state_.get();
}
btDefaultMotionState* RigidBody::GetMutableMotionState() {
	return motion_state_.get();
}

btTransform RigidBody::GetTransform() const {
	btTransform transform;
	motion_state_->getWorldTransform(transform);
	return transform;
}
void RigidBody::SetTransform(btTransform new_transform) {
	motion_state_->setWorldTransform(new_transform);
}

void RigidBody::MakeCollideable(bool collideable) const {
	if (collideable) {
		body_->setCollisionFlags(body_->getCollisionFlags() |
			btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	} else {
		body_->setCollisionFlags(body_->getCollisionFlags() &
			~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

bool RigidBody::IsCollideable() const {
	return body_->getCollisionFlags() &
		btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK;
}

namespace shapes {
	Shape MakeSphere(btScalar radius) {
		return make_unique<btSphereShape>(radius);
	}

	Shape MakePlane(btVector3 normal, btScalar plane_constant) {
		return make_unique<btStaticPlaneShape>(normal, plane_constant);
	}

	Shape MakePlane(btVector3 normal, btVector3 point_in_plane) {
		return MakePlane(normal, normal.dot(point_in_plane));
	}
}  // shapes

namespace poses {
	btQuaternion GetQuaternion(const Quaternion& quat) {
		return btQuaternion(quat.x, quat.y, quat.z, quat.w);
	}
	btVector3 GetVector3(const Location& loc) {
		return btVector3(loc[0], loc[1], loc[2]);
	}
	btTransform GetTransform(const Pose& pose) {
		return btTransform(GetQuaternion(pose.orientation_), GetVector3(pose.location_));
	}

	Quaternion GetQuaternion(const btQuaternion& quat) {
		btQuaternionData quat_data;
		quat.serialize(quat_data);
		return Quaternion(quat_data.m_floats);
	}
	Location GetLocation(const btVector3& loc) {
		return Location(loc.x(), loc.y(), loc.z());
	}
	Pose GetPose(const btTransform& pose) {
		return Pose(GetLocation(pose.getOrigin()), GetQuaternion(pose.getRotation()));
	}
}  // poses
}  // bullet

extern ContactProcessedCallback gContactProcessedCallback;
extern ContactAddedCallback gContactAddedCallback;

namespace game_scene {
CollisionInfo collision_info::Make(
	bullet::CollisionComponent cc_a, const btCollisionObjectWrapper* wrapper_a,
	bullet::CollisionComponent cc_b, const btCollisionObjectWrapper* wrapper_b) {
	if (cc_b < cc_a) {
		return make_pair(cc_a, make_tuple(cc_b, wrapper_a, wrapper_b));
	}
	return make_pair(cc_b, make_tuple(cc_a, wrapper_b, wrapper_a));
}

PhysicsSimulationImpl::PhysicsSimulationImpl(bullet::World world) : world_(std::move(world)) {}

void PhysicsSimulationImpl::HandleTimeTick(const commands::TimeTick& args) {
	world_.Step(args.duration_ / 1000.0f);
}

void PhysicsSimulationImpl::PreTimeTick() {
	current_collisions_.clear();
	MakeCallbackOwner();
}

void PhysicsSimulationImpl::PostTimeTick() {
	for (const CollisionInfo& collision_info : current_collisions_) {
		if (collision_info.first.actor_ != ActorId::UnsetId) {
		}
	}
}

PhysicsSimulationImpl* PhysicsSimulationImpl::callback_owner = nullptr;

void PhysicsSimulationImpl::MakeCallbackOwner() {
	callback_owner = this;

	gContactProcessedCallback = ContactProcessedCallbackDelegater;
	gContactAddedCallback = ContactAddedCallbackDelegater;
}

bool PhysicsSimulationImpl::ContactProcessedCallback(
	btManifoldPoint& cp,
	void* body0, void* body1) {
	std::cout << "PROCESSED" << std::endl;
	btCollisionObjectWrapper* colObj0Wrap = static_cast<btCollisionObjectWrapper*>(body0);
	btCollisionObjectWrapper* colObj1Wrap = static_cast<btCollisionObjectWrapper*>(body1);
	return true;
}

bool PhysicsSimulationImpl::ContactAddedCallback(
	btManifoldPoint& cp,
	const btCollisionObjectWrapper* colObj0Wrap,
	int partId0, int index0,
	const btCollisionObjectWrapper* colObj1Wrap,
	int partId1, int index1) {
	std::cout << "ADDED" << std::endl;
	return true;
}

bool PhysicsSimulationImpl::ContactProcessedCallbackDelegater(
	btManifoldPoint& cp,
	void* body0, void* body1) {
	return callback_owner->ContactProcessedCallback(cp, body0, body1);
}

bool PhysicsSimulationImpl::ContactAddedCallbackDelegater(
	btManifoldPoint& cp,
	const btCollisionObjectWrapper* colObj0Wrap,
	int partId0, int index0,
	const btCollisionObjectWrapper* colObj1Wrap,
	int partId1, int index1) {
	return callback_owner->ContactAddedCallback(
		cp,
		colObj0Wrap, partId0, index0,
		colObj1Wrap, partId1, index1);
}

}  // game_scene