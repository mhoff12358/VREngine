#pragma once

#include "stdafx.h"

#include "VRBackend/Pose.h"
#include <btBulletDynamicsCommon.h>
#include <btBulletCollisionCommon.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>

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

class Shape {
public:
    Shape();
	Shape(Shape&& shape);
	Shape& operator=(Shape&&) = default;
    Shape(unique_ptr<btCollisionShape> shape);

    unique_ptr<btCollisionShape> shape_;

	static Shape MakeSphere(btScalar radius);
	static Shape MakePlane(btVector3 normal, btScalar plane_constant = 0);
	static Shape MakePlane(btVector3 normal, btVector3 point_in_plane);
  static Shape MakeCylinder(btVector3 half_extents);
	static Shape MakeBox(btVector3 half_extents);
};

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

class CollisionObject {
public:
	enum CollisionObjectType : unsigned char {
		NORMAL = 1,
		GHOST = 2,
		PAIR_CACHING_GHOST = 3,
	};

	CollisionObject();
	CollisionObject(CollisionObjectType type, Shape shape, btTransform transform = btTransform());

	bool GetFilled() const;

	btCollisionObject* GetCollisionObject() const;

	void SetTransform(btTransform new_transform);
	btTransform GetTransform() const;

  void SetShape(Shape shape);
  const Shape& GetShape() const;

  static optional<CollisionObject&> GetFromBullet(const btCollisionObject* bullet_object);

protected:
	unique_ptr<btCollisionObject> object_;
	Shape shape_;
};

class RigidBody : public CollisionObject {
public:
	typedef std::function<void(const Pose&, const Pose&)> NewPoseCallback;
	class MotionState : public btMotionState {
	public:
		MotionState(btTransform initial_transform);

		// These functions are meant to be called by bullet. getWorldTransform is supposed to
		// tell bullet what the current transform is. setWorldTransform is called whenever bullet
		// updates the position of the object.
		void getWorldTransform(btTransform& worldTrans) const override;
    void setWorldTransform(const btTransform& worldTrans) override;

		// This function is meant to be called by the scene to set a kinematic object's position.
		void PushNewTransform(const btTransform& new_transform);

		void SetCallback();
		void SetCallback(NewPoseCallback callback);

		void SetKinematic(bool kinematic);

	private:
		void SetTransformAndCallback(const btTransform& new_transform);

		bool kinematic_;
		btTransform current_transform_;
		NewPoseCallback callback_;
	};

  struct InteractionType {
    InteractionType(btScalar mass = 1.0f, btVector3 intertia = btVector3(0, 0, 0))
      : mass_(mass), inertia_(intertia) {}
    InteractionType(bool kinematic)
      : mass_(0), inertia_(btVector3(0, 0, 0)), kinematic_(kinematic) {}

    btScalar mass_;
    btVector3 inertia_;
    bool kinematic_;

    bool IsDynamic() const {
      return mass_ != 0;
    }

    bool IsKinematic() const {
      return (mass_ == 0) && kinematic_;
    }

    bool IsStatic() const {
      return (mass_ == 0) && !kinematic_;
    }

    static InteractionType Kinematic() { return InteractionType(true); }
    static InteractionType Static() { return InteractionType(false); }
  };

	RigidBody();
	RigidBody(
		Shape shape,
		unique_ptr<btMotionState> starting_motion_state,
		btScalar friction = 1.0f,
		InteractionType interaction_type = InteractionType::Static());
	RigidBody(
		Shape shape,
		btTransform transform,
		btScalar friction = 1.0f,
		InteractionType interaction_type = InteractionType::Static());
	RigidBody(RigidBody&&) = default;
	RigidBody& operator=(RigidBody&& other) = default;

	bool GetFilled() const;

	btRigidBody* GetBody() const;

	const RigidBodyPayload& GetPayload() const;
	void SetPayload(RigidBodyPayload* payload);

	const btMotionState& GetMotionState() const;
	btMotionState* GetMutableMotionState();

	void MakeStatic();
	void MakeDynamic();
	void RemoveKinematic();
	void MakeKinematic();

	void SetPoseUpdatedCallback(NewPoseCallback callback);
 
	void MakeCollideable(bool collideable = true) const;
	bool IsCollideable() const;

private:
	unique_ptr<btMotionState> motion_state_;
	btScalar stored_mass_ = 0.0f;
};

class CollisionResult {
public:
  CollisionResult() = default;

  void AddObject(const btCollisionObject*);

  bool CollisionFound() const;
  const vector<const btCollisionObject*>& GetRawObjects() const;
  vector<CollisionObject*> GetExistingWrappedObjects() const;
  vector<optional<CollisionObject&>> GetWrappedObjects() const;

private:
  vector<const btCollisionObject*> collisions_;
};

class World {
public:
	World(Config config);

	void Step(btScalar time_step);

	void AddRigidBody(const RigidBody& body);
	void AddRigidBody(btRigidBody* body);
	void RemoveRigidBody(const RigidBody& body);
	void RemoveRigidBody(btRigidBody* body);

	void AddCollisionObject(const CollisionObject& object);
	void AddCollisionObject(btCollisionObject* object);
	void RemoveCollisionObject(const CollisionObject& object);
	void RemoveCollisionObject(btCollisionObject* object);

  CollisionResult CheckCollision(const CollisionObject& object);
  bool CheckCollisionExistsPair(const CollisionObject& object_0, const CollisionObject& object_1) const;

	btDiscreteDynamicsWorld* Get();

private:
	Config config_;
	unique_ptr<btDiscreteDynamicsWorld> world_;
	unsigned int substep_limit_ = 10;
};

namespace poses {
	btQuaternion GetQuaternion(const Quaternion& quat);
	btVector3 GetVector3(const Location& loc);
	btVector3 GetVector3(const Scale& scale);
	btTransform GetTransform(const Pose& pose);
	Quaternion GetQuaternion(const btQuaternion& quat);
	Location GetLocation(const btVector3& loc);
	Pose GetPose(const btTransform& pose);
}  // poses
}  // bullet

