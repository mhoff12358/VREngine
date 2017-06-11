#include "stdafx.h"
#include "BulletInternals.h"

namespace bullet {
CollisionInfo collision_info::Make(
	bullet::CollisionComponent cc_a, const btCollisionObjectWrapper* wrapper_a,
	bullet::CollisionComponent cc_b, const btCollisionObjectWrapper* wrapper_b) {
	if (cc_b < cc_a) {
		return make_pair(cc_a, make_tuple(cc_b, wrapper_a, wrapper_b));
	}
	return make_pair(cc_b, make_tuple(cc_a, wrapper_b, wrapper_a));
}


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

Shape::Shape() : shape_(nullptr) {}

Shape::Shape(Shape&& shape) : shape_(std::move(shape.shape_)) {}

Shape::Shape(unique_ptr<btCollisionShape> shape) : shape_(std::move(shape)) {}

Shape Shape::MakeSphere(btScalar radius) {
	return Shape(make_unique<btSphereShape>(radius));
}

Shape Shape::MakePlane(btVector3 normal, btScalar plane_constant) {
	return Shape(make_unique<btStaticPlaneShape>(normal, plane_constant));
}

Shape Shape::MakePlane(btVector3 normal, btVector3 point_in_plane) {
	return MakePlane(normal, normal.dot(point_in_plane));
}

RigidBody::MotionState::MotionState(btTransform initial_transform)
	: kinematic_(false), current_transform_(initial_transform), callback_() {

}

void RigidBody::MotionState::getWorldTransform(btTransform& worldTrans) const {
	worldTrans = current_transform_;
}
void RigidBody::MotionState::setWorldTransform(const btTransform& worldTrans) {
	if (kinematic_) {
		std::cout << "Bullet is trying to set the world transformation on a kinematic object" << std::endl;
		return;
	}
	SetTransformAndCallback(worldTrans);
}

void RigidBody::MotionState::PushNewTransform(const btTransform& new_transform) {
	SetTransformAndCallback(new_transform);
}

void RigidBody::MotionState::SetCallback() {
	callback_ = NewPoseCallback();
}
void RigidBody::MotionState::SetCallback(NewPoseCallback callback) {
	callback_ = callback;
}

void RigidBody::MotionState::SetKinematic(bool kinematic) {
	kinematic_ = kinematic;
}

void RigidBody::MotionState::SetTransformAndCallback(const btTransform& new_transform) {
	Pose old_pose = poses::GetPose(current_transform_);
	current_transform_ = new_transform;
	if (callback_) {
		callback_(old_pose, poses::GetPose(current_transform_));
	}
}

RigidBody::RigidBody() :
	CollisionObject(), motion_state_(nullptr) {}

RigidBody::RigidBody(
	Shape shape,
	btTransform transform,
	InteractionType interaction_type) :
	RigidBody(std::move(shape), make_unique<MotionState>(transform), interaction_type) {}

RigidBody::RigidBody(
	Shape shape,
	unique_ptr<btMotionState> starting_motion_state,
	InteractionType interaction_type) :
	motion_state_(std::move(starting_motion_state))
{
  shape_ = std::move(shape);
	if (interaction_type.IsDynamic()) {
		shape_.shape_->calculateLocalInertia(interaction_type.mass_, interaction_type.inertia_);
	}
  btRigidBody::btRigidBodyConstructionInfo construction_info(interaction_type.mass_, motion_state_.get(), shape_.shape_.get(), interaction_type.inertia_);
	object_ = make_unique<btRigidBody>(construction_info);
  if (interaction_type.IsKinematic()) {
    MakeKinematic();
  }
}

bool RigidBody::GetFilled() const {
	return shape_.shape_.get() != nullptr;
}

btRigidBody* RigidBody::GetBody() const {
	return static_cast<btRigidBody*>(object_.get());
}

const RigidBodyPayload& RigidBody::GetPayload() const {
	return *static_cast<RigidBodyPayload*>((shape_.shape_->getUserPointer()));
}

void RigidBody::SetPayload(RigidBodyPayload* payload) {
	shape_.shape_->setUserPointer(static_cast<void*>(payload));
}

const btMotionState& RigidBody::GetMotionState() const {
	return *motion_state_.get();
}
btMotionState* RigidBody::GetMutableMotionState() {
	return motion_state_.get();
}

void RigidBody::MakeStatic() {
	RemoveKinematic();
	btScalar inv_mass = GetBody()->getInvMass();
	if (inv_mass) {
		GetBody()->setMassProps(0.0f, GetBody()->getLocalInertia());
		stored_mass_ = 1.0f / inv_mass;
	}
}

void RigidBody::MakeDynamic() {
	RemoveKinematic();
	if (stored_mass_) {
		GetBody()->setMassProps(stored_mass_, GetBody()->getLocalInertia());
	}
}

void RigidBody::RemoveKinematic() {
	object_->setCollisionFlags(object_->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
	if (MotionState* downcast_motion_state = dynamic_cast<MotionState*>(motion_state_.get())) {
		downcast_motion_state->SetKinematic(false);
	}
}

void RigidBody::MakeKinematic() {
	object_->setCollisionFlags(object_->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
	if (MotionState* downcast_motion_state = dynamic_cast<MotionState*>(motion_state_.get())) {
		downcast_motion_state->SetKinematic(true);
	}
}

void RigidBody::SetPoseUpdatedCallback(NewPoseCallback callback) {
	if (MotionState* downcast_motion_state = dynamic_cast<MotionState*>(motion_state_.get())) {
		downcast_motion_state->SetCallback(callback);
	}
}

void RigidBody::MakeCollideable(bool collideable) const {
	if (collideable) {
		object_->setCollisionFlags(object_->getCollisionFlags() |
			btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	} else {
		object_->setCollisionFlags(object_->getCollisionFlags() &
			~btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
	}
}

bool RigidBody::IsCollideable() const {
	return (object_->getCollisionFlags() &
		btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK) != 0;
}

CollisionObject::CollisionObject() :
	object_(nullptr), shape_(nullptr) {}

CollisionObject::CollisionObject(CollisionObjectType type, Shape shape, btTransform transform) :
	object_(nullptr), shape_(std::move(shape)) {
	switch (type) {
	case NORMAL:
		object_ = make_unique<btCollisionObject>();
	case GHOST:
		object_ = make_unique<btGhostObject>();
	case PAIR_CACHING_GHOST:
		object_ = make_unique<btPairCachingGhostObject>();
	}
	object_->setCollisionShape(shape_.shape_.get());
	object_->setCollisionFlags(btCollisionObject::CF_NO_CONTACT_RESPONSE);
	object_->setWorldTransform(transform);
}

bool CollisionObject::GetFilled() const {
	return object_.get() != nullptr;
}

btCollisionObject* CollisionObject::GetCollisionObject() const {
	return object_.get();
}

void CollisionObject::SetTransform(btTransform new_transform) {
	object_->setWorldTransform(new_transform);
}
btTransform CollisionObject::GetTransform() const {
	return object_->getWorldTransform();
}

void CollisionObject::SetShape(Shape shape) {
  shape_ = std::move(shape);
  object_->setCollisionShape(shape_.shape_.get());
}

const Shape& CollisionObject::GetShape() const {
  return shape_;
}

optional<CollisionObject&> CollisionObject::GetFromBullet(const btCollisionObject* bullet_object) {
  void* result = bullet_object->getUserPointer();
  if (result) {
    return *static_cast<CollisionObject*>(result);
  }
  return optional<CollisionObject&>();
}

void CollisionResult::AddObject(const btCollisionObject* new_object) {
  collisions_.push_back(new_object);
}

bool CollisionResult::CollisionFound() const {
  return !collisions_.empty();
}

const vector<const btCollisionObject*>& CollisionResult::GetRawObjects() const {
  return collisions_;
}

vector<CollisionObject*> CollisionResult::GetExistingWrappedObjects() const {
  vector<CollisionObject*> result;
  for (const btCollisionObject* raw_object : collisions_) {
    if (optional<CollisionObject&> possible_wrap = CollisionObject::GetFromBullet(raw_object)) {
      result.push_back(&possible_wrap.get());
    }
  }
  return result;
}

vector<optional<CollisionObject&>> CollisionResult::GetWrappedObjects() const {
  vector<optional<CollisionObject&>> result;
  for (const btCollisionObject* raw_object : collisions_) {
    result.push_back(CollisionObject::GetFromBullet(raw_object));
  }
  return result;
}

World::World(Config config) :
	config_(std::move(config)),
	world_(new btDiscreteDynamicsWorld(
		config_.collision_dispatch_.get(),
		config_.broadphase_.get(),
		config_.solver_.get(),
		config_.collision_config_.get())) {
	world_->setGravity(btVector3(0, -10, 0));
	world_->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
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

void World::AddCollisionObject(const CollisionObject& object) {
	AddCollisionObject(object.GetCollisionObject());
}
void World::AddCollisionObject(btCollisionObject* object) {
	
	world_->addCollisionObject(object,0x1,0x1);
}
void World::RemoveCollisionObject(const CollisionObject& object) {
	RemoveCollisionObject(object.GetCollisionObject());
}
void World::RemoveCollisionObject(btCollisionObject* object) {
	world_->removeCollisionObject(object);
}

struct CreateCollisionResult : public btCollisionWorld::ContactResultCallback {
  CreateCollisionResult(const CollisionObject& object) {
    collision_object_ = object.GetCollisionObject();
  }

  btScalar addSingleResult(
    btManifoldPoint& cp,
    const btCollisionObjectWrapper* colObj0Wrap,
    int partId0, int index0,
    const btCollisionObjectWrapper* colObj1Wrap,
    int partId1, int index1) override {
    const btCollisionObject* other_object_0 = colObj0Wrap->getCollisionObject();
    const btCollisionObject* other_object_1 = colObj1Wrap->getCollisionObject();
    if (other_object_0 != collision_object_) {
      result_.AddObject(other_object_0);
    } else {
      assert(other_object_1 != collision_object_);
      result_.AddObject(other_object_1);
    }
    return 0.0f;
  }

  const btCollisionObject* collision_object_;
  CollisionResult result_;
};

struct CreateCollisionOccurance : public btCollisionWorld::ContactResultCallback {
  bool result_ = false;
  btScalar addSingleResult(
    btManifoldPoint& cp,
    const btCollisionObjectWrapper* colObj0Wrap,
    int partId0, int index0,
    const btCollisionObjectWrapper* colObj1Wrap,
    int partId1, int index1) override {
    result_ = true;
    return 0.0f;
  }
};

CollisionResult World::CheckCollision(const CollisionObject& object) {
  CreateCollisionResult collision_result_generator(object);
  world_->contactTest(object.GetCollisionObject(), collision_result_generator);
  return collision_result_generator.result_;
}

bool World::CheckCollisionExistsPair(const CollisionObject& object_0, const CollisionObject& object_1) const {
  CreateCollisionOccurance collision_result_generator;
  world_->contactPairTest(object_0.GetCollisionObject(), object_1.GetCollisionObject(), collision_result_generator);
  return collision_result_generator.result_;
}

btDiscreteDynamicsWorld* World::Get() {
	return world_.get();
}

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

