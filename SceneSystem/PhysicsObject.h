#pragma once

#include "stdafx.h"

#include "PhysXInternal.h"

#include "CommandArgs.h"
#include "QueryArgs.h"
#include "QueryResult.h"
#include "PhysicsInteractions.h"
#include "Poseable.h"

namespace game_scene {

namespace {
struct PhysicsObjectComponent {
	PhysicsObjectComponent() : body_(nullptr), enabled_(false) {}
	PhysicsObjectComponent(PxRigidActor* body, bool enabled) :
		body_(body), enabled_(enabled) {}

	void SetKinematicTargeting(bool targeting) { kinematic_targeting_ = targeting; }
	void SetPose(const Pose& pose) {
		auto body = body_->is<PxRigidDynamic>();
		if (body && (body->getRigidBodyFlags() | PxRigidBodyFlag::eKINEMATIC) && kinematic_targeting_) {
			body->setKinematicTarget(poses::ToTransform(pose));
		} else {
			body_->setGlobalPose(poses::ToTransform(pose));
		}
	}

	PxRigidActor* body_;
	bool kinematic_targeting_ = false;
	bool enabled_;
};
}

class PhysicsObjectCommand {
public:
	DECLARE_COMMAND(PhysicsObjectCommand, ADD_RIGID_BODY);
	DECLARE_COMMAND(PhysicsObjectCommand, REMOVE_RIGID_BODY);
	DECLARE_COMMAND(PhysicsObjectCommand, ADD_UPDATED_CALLBACK);
};

struct AddRigidBody : public CommandArgs {
	AddRigidBody(string name, bool enabled, PxRigidActor* rigid_body) :
		CommandArgs(PhysicsObjectCommand::ADD_RIGID_BODY),
		name_(name),
		enabled_(enabled),
		rigid_body_(std::move(rigid_body)) {}
	AddRigidBody(bool enabled, PxRigidActor* rigid_body) :
		AddRigidBody("", enabled, std::move(rigid_body)) {}
	AddRigidBody(string name, PxRigidActor* rigid_body) :
		AddRigidBody(name, true, std::move(rigid_body)) {}
	AddRigidBody(PxRigidActor* rigid_body) :
		AddRigidBody("", true, std::move(rigid_body)) {}

	string name_;
	bool enabled_;
	PxRigidActor* rigid_body_;
};

struct RemoveRigidBody : public CommandArgs {
	RemoveRigidBody(string name) :
		CommandArgs(PhysicsObjectCommand::REMOVE_RIGID_BODY), name_(name) {}

	string name_;
};

struct AddUpdatedCallback : public CommandArgs {
	AddUpdatedCallback(game_scene::ActorCallback callback)
		: CommandArgs(PhysicsObjectCommand::ADD_UPDATED_CALLBACK), callback_(callback) {}

	game_scene::ActorCallback callback_;
};

class PhysicsObjectQuery {
public:
  DECLARE_QUERY(PhysicsObjectQuery, GET_RIGID_BODIES);
  DECLARE_QUERY(PhysicsObjectQuery, CHECK_COLLISION);
};

struct GetRigidBodies : public QueryArgs {
  GetRigidBodies() : QueryArgs(PhysicsObjectQuery::GET_RIGID_BODIES) {}
};

struct GetRigidBodiesResult : public QueryResult {
	GetRigidBodiesResult(PxRigidActor* rigid_body) :
		QueryResult(PhysicsObjectQuery::GET_RIGID_BODIES), rigid_bodies_() {
		rigid_bodies_.push_back(rigid_body);
	}
	GetRigidBodiesResult(vector<PxRigidActor*> rigid_bodies) :
		QueryResult(PhysicsObjectQuery::GET_RIGID_BODIES), rigid_bodies_(std::move(rigid_bodies)) {
	}

	vector<PxRigidActor*> rigid_bodies_;
};

/*struct CheckCollisionQuery : public QueryArgs{
  CheckCollisionQuery(const bullet::World& world, const bullet::CollisionObject& collision_object) :
    QueryArgs(PhysicsObjectQuery::CHECK_COLLISION),
    world_(world),
    collision_object_(collision_object) {}

  const bullet::World& world_;
  const bullet::CollisionObject& collision_object_;
};

struct CheckCollisionResult : public QueryResult {
  CheckCollisionResult(bool collision) :
    QueryResult(PhysicsObjectQuery::CHECK_COLLISION),
    collision_(collision) {}

  bool collision_;
};*/

namespace actors {

template <typename ActorBase>
class PhysicsObject : public ActorBase {
public:
	PhysicsObject() : ActorBase(), rigid_body_(), updated_callback_() {}

	void HandleCommand(CommandArgs& args) {
		switch (args.Type()) {
		case PhysicsObjectCommand::ADD_RIGID_BODY:
			SetRigidBody(std::move(dynamic_cast<AddRigidBody&>(args)));
			break;
		case PhysicsObjectCommand::REMOVE_RIGID_BODY:
			ClearRigidBody();
			break;
		/*case PhysicsInteractionCommand::RIGID_BODY_MOVED:
			RigidBodyPoseUpdated();
			break;*/
		case commands::PoseableCommand::ACCEPT_NEW_POSE:
			HandleNewPose(dynamic_cast<commands::AcceptNewPose&>(args));
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) const {
		switch (args.Type()) {
		case PhysicsObjectQuery::GET_RIGID_BODIES:
			if (rigid_body_.enabled_) {
				return make_unique<GetRigidBodiesResult>(rigid_body_.body_);
			} else {
				return make_unique<GetRigidBodiesResult>(vector<PxRigidActor*>());
			}
		default:
			return make_unique<QueryResult>(QueryType::EMPTY);
		}
	}

	static string GetName() {
		return "PhysicsObject-" + ActorBase::GetName();
	}

  PxRigidActor* GetRigidBody(string name) {
    return rigid_body_.body_;
  }

private:
	/*PxRigidActor*::NewPoseCallback MakeRigidBodyCallback(string name) {
    return [this, name](const Pose& old_pose, const Pose& new_pose) { poseable::PushNewPose(*this, name, new_pose); };
	}*/

	void SetRigidBody(AddRigidBody& args) {
		rigid_body_name_ = args.name_;
		rigid_body_ = PhysicsObjectComponent(args.rigid_body_, args.enabled_);
		//rigid_body_.body_.SetPoseUpdatedCallback(MakeRigidBodyCallback(rigid_body_name_));
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void ClearRigidBody() {
		rigid_body_ = PhysicsObjectComponent();
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void RigidBodyPoseUpdated() {
		poseable::PushNewPose(
			*this, 
			rigid_body_name_,
			poses::ToPose(rigid_body_.body_->getGlobalPose()));
	}

	void HandleNewPose(commands::AcceptNewPose& args) {
		if (args.pose_source_ != GetNameVirt()) {
			rigid_body_.SetPose(args.new_pose_);
		}
	}

	string rigid_body_name_;
	PhysicsObjectComponent rigid_body_;
	game_scene::ActorCallback updated_callback_;
};

template <typename ActorBase>
class PhysicsObjectCollection : public ActorBase {
public:
	PhysicsObjectCollection() : ActorBase(), rigid_bodies_() {}

	void HandleCommand(CommandArgs& args) {
		switch (args.Type()) {
		case PhysicsObjectCommand::ADD_RIGID_BODY:
			AddNewRigidBody(dynamic_cast<AddRigidBody&>(args));
			break;
		case PhysicsObjectCommand::REMOVE_RIGID_BODY:
			RemoveExistingRigidBody(dynamic_cast<RemoveRigidBody&>(args));
			break;
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) const {
		switch (args.Type()) {
		case PhysicsObjectQuery::GET_RIGID_BODIES:
		{
			vector<PxRigidActor*> bodies;
			for (const auto& rigid_body : rigid_bodies_) {
				if (rigid_body.second.enabled_) {
					bodies.push_back(rigid_body.second.body_);
				}
			}
			return make_unique<GetRigidBodiesResult>(bodies);
		}
		default:
			return make_unique<QueryResult>(QueryType::EMPTY);
		}
	}

	static string GetName() {
		return "PhysicsObjectCollection-" + ActorBase::GetName();
	}

private:
	void AddNewRigidBody(AddRigidBody& args) {
		rigid_bodies_[args.name_] = PhysicsObjectComponent(args.rigid_body_, args.enabled_);
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void RemoveExistingRigidBody(RemoveRigidBody& args) {
		rigid_bodies_.erase(args.name_);
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void RigidBodyPoseUpdated() {
		for (const auto& named_rigid_body : rigid_bodies_) {
			poseable::PushNewPose(*this, 
				named_rigid_body.first,
				poses::ToPose(named_rigid_body.second.body_->getGlobalPose()));
		}
	}

	void HandleNewPose(commands::AcceptNewPose& args) {
		if (args.pose_source_ != GetNameVirt()) {
			auto rigid_body_iter = rigid_bodies_.find(args.name_);
			if (rigid_body_iter == rigid_bodies_.end()) {
				std::cout << "Attempting to set a new pose of a rigid body that doesn't exist." << std::endl;
				return;
			}
			rigid_body_iter->second.SetPose(args.new_pose_);
		}
	}

	map<string, PhysicsObjectComponent> rigid_bodies_;
	game_scene::ActorCallback updated_callback_;
};

template class PhysicsObject<Poseable<ActorImpl>>;
template class PhysicsObject<ActorImpl>;
template class PhysicsObjectCollection<Poseable<ActorImpl>>;
template class PhysicsObjectCollection<ActorImpl>;

}
}

