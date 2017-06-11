#pragma once

#include "stdafx.h"

#include "BulletInternals.h"

#include "CommandArgs.h"
#include "QueryArgs.h"
#include "QueryResult.h"
#include "PhysicsInteractions.h"
#include "Poseable.h"

namespace game_scene {

namespace {
struct PhysicsObjectComponent {
	PhysicsObjectComponent() : body_(), enabled_(false) {}
	PhysicsObjectComponent(bullet::RigidBody body, bool enabled) :
		body_(std::move(body)), enabled_(enabled) {}

	bullet::RigidBody body_;
	bool enabled_;
};
}

namespace commands {

class PhysicsObjectCommand {
public:
	DECLARE_COMMAND(PhysicsObjectCommand, ADD_RIGID_BODY);
	DECLARE_COMMAND(PhysicsObjectCommand, REMOVE_RIGID_BODY);
	DECLARE_COMMAND(PhysicsObjectCommand, MODIFY_RIGID_BODY);
	DECLARE_COMMAND(PhysicsObjectCommand, ADD_UPDATED_CALLBACK);
};

struct AddRigidBody : public CommandArgs {
	AddRigidBody(string name, bool enabled, bullet::RigidBody rigid_body) :
		CommandArgs(PhysicsObjectCommand::ADD_RIGID_BODY),
		name_(name),
		enabled_(enabled),
		rigid_body_(std::move(rigid_body)) {}
	AddRigidBody(bool enabled, bullet::RigidBody rigid_body) :
		AddRigidBody("", enabled, std::move(rigid_body)) {}
	AddRigidBody(string name, bullet::RigidBody rigid_body) :
		AddRigidBody(name, true, std::move(rigid_body)) {}
	AddRigidBody(bullet::RigidBody rigid_body) :
		AddRigidBody("", true, std::move(rigid_body)) {}

	string name_;
	bool enabled_;
	bullet::RigidBody rigid_body_;
};

struct RemoveRigidBody : public CommandArgs {
	RemoveRigidBody(string name) :
		CommandArgs(PhysicsObjectCommand::REMOVE_RIGID_BODY), name_(name) {}

	string name_;
};

struct ModifyRigidBody : public CommandArgs {
  typedef std::function<bool(bullet::RigidBody& body)> ModifyFunction;
	ModifyRigidBody(ModifyFunction modifier) :
		CommandArgs(PhysicsObjectCommand::MODIFY_RIGID_BODY), modifier_(modifier) {}

	bool ApplyModifier(PhysicsObjectComponent& comp) {
    return modifier_(comp.body_);
	}

	ModifyFunction modifier_;
	bool modify_all_;
	vector<string> names_to_modify_;
};

struct AddUpdatedCallback : public CommandArgs {
	AddUpdatedCallback(game_scene::ActorCallback callback)
		: CommandArgs(PhysicsObjectCommand::ADD_UPDATED_CALLBACK), callback_(callback) {}

	game_scene::ActorCallback callback_;
};

}  // commands

namespace queries {

class PhysicsObjectQuery {
public:
	DECLARE_QUERY(PhysicsObjectQuery, GET_RIGID_BODIES);
  DECLARE_QUERY(PhysicsObjectQuery, CHECK_COLLISION);
};

struct GetRigidBodies : public QueryArgs {
  GetRigidBodies() : QueryArgs(PhysicsObjectQuery::GET_RIGID_BODIES) {}
};

struct GetRigidBodiesResult : public QueryResult {
	GetRigidBodiesResult(const bullet::RigidBody& rigid_body) :
		QueryResult(PhysicsObjectQuery::GET_RIGID_BODIES), rigid_bodies_() {
		rigid_bodies_.push_back(&rigid_body);
	}
	GetRigidBodiesResult(vector<const bullet::RigidBody*> rigid_bodies) :
		QueryResult(PhysicsObjectQuery::GET_RIGID_BODIES), rigid_bodies_(std::move(rigid_bodies)) {
	}

	vector<const bullet::RigidBody*> rigid_bodies_;
};

struct CheckCollisionQuery : public QueryArgs{
  CheckCollisionQuery(const bullet::CollisionObject& collision_object) :
    QueryArgs(PhysicsObjectQuery::CHECK_COLLISION),
    collision_object_(collision_object) {}

  const bullet::CollisionObject& collision_object_;
};

struct CheckCollisionResult : public QueryResult {
  CheckCollisionResult(bool collision) :
    QueryResult(PhysicsObjectQuery::CHECK_COLLISION),
    collision_(collision) {}

  bool collision_;
};

}  // queries

namespace actors {

template <typename ActorBase>
class PhysicsObject : public ActorBase {
public:
	PhysicsObject() : ActorBase(), rigid_body_(), updated_callback_() {}

	void HandleCommand(CommandArgs& args) {
		switch (args.Type()) {
		case commands::PhysicsObjectCommand::ADD_RIGID_BODY:
			SetRigidBody(std::move(dynamic_cast<commands::AddRigidBody&>(args)));
			break;
		case commands::PhysicsObjectCommand::REMOVE_RIGID_BODY:
			ClearRigidBody();
			break;
		case commands::PhysicsObjectCommand::MODIFY_RIGID_BODY:
			ModifyRigidBody(dynamic_cast<commands::ModifyRigidBody&>(args));
		case commands::PhysicsInteractionCommand::RIGID_BODY_MOVED:
			RigidBodyPoseUpdated();
			break;
		case commands::PoseableCommand::ACCEPT_NEW_POSE:
			HandleNewPose(dynamic_cast<commands::AcceptNewPose&>(args));
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) const {
		switch (args.Type()) {
		case queries::PhysicsObjectQuery::GET_RIGID_BODIES:
			if (rigid_body_.enabled_) {
				return make_unique<queries::GetRigidBodiesResult>(rigid_body_.body_);
			} else {
				return make_unique<queries::GetRigidBodiesResult>(vector<const bullet::RigidBody*>());
			}
    case queries::PhysicsObjectQuery::CHECK_COLLISION:
      return CheckCollision(dynamic_cast<const queries::CheckCollisionQuery&>(args));
		default:
			return make_unique<QueryResult>(QueryType::EMPTY);
		}
	}

	static string GetName() {
		return "PhysicsObject-" + ActorBase::GetName();
	}

  bullet::RigidBody& GetRigidBody(string name) {
    return rigid_body_.body_;
  }

private:
	bullet::RigidBody::NewPoseCallback MakeRigidBodyCallback(string name) {
    return [this, name](const Pose& old_pose, const Pose& new_pose) { poseable::PushNewPose(*this, name, new_pose); };
	}

	void SetRigidBody(commands::AddRigidBody& args) {
		rigid_body_name_ = args.name_;
		rigid_body_ = PhysicsObjectComponent(std::move(args.rigid_body_), args.enabled_);
		rigid_body_.body_.SetPoseUpdatedCallback(MakeRigidBodyCallback(rigid_body_name_));
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

	void ModifyRigidBody(commands::ModifyRigidBody& args) {
		bool changed = args.ApplyModifier(rigid_body_);
		if (changed && updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void RigidBodyPoseUpdated() {
		poseable::PushNewPose(*this, 
			rigid_body_name_,
			bullet::poses::GetPose(rigid_body_.body_.GetTransform()));
	}

	void HandleNewPose(commands::AcceptNewPose& args) {
		if (args.pose_source_ != GetNameVirt()) {
			rigid_body_.body_.SetTransform(bullet::poses::GetTransform(args.new_pose_));
		}
	}

  unique_ptr<queries::CheckCollisionResult> CheckCollision(const queries::CheckCollisionQuery& args) const {
    const bullet::CollisionObject& other_object = args.collision_object_;
    return make_unique<queries::CheckCollisionResult>(rigid_body_.enabled_ && rigid_body_.body_.GetBody()->checkCollideWith(other_object.GetCollisionObject()));
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
		case commands::PhysicsObjectCommand::ADD_RIGID_BODY:
			AddNewRigidBody(dynamic_cast<commands::AddRigidBody&>(args));
			break;
		case commands::PhysicsObjectCommand::REMOVE_RIGID_BODY:
			RemoveExistingRigidBody(dynamic_cast<commands::RemoveRigidBody&>(args));
			break;
		case commands::PhysicsObjectCommand::MODIFY_RIGID_BODY:
			ModifyRigidBody(dynamic_cast<commands::ModifyRigidBody&>(args));
		case commands::PhysicsInteractionCommand::RIGID_BODY_MOVED:
			RigidBodyPoseUpdated();
			break;
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) const {
		switch (args.Type()) {
		case queries::PhysicsObjectQuery::GET_RIGID_BODIES:
		{
			vector<const bullet::RigidBody*> bodies;
			for (const auto& rigid_body : rigid_bodies_) {
				if (rigid_body.second.enabled_) {
					bodies.push_back(&rigid_body.second.body_);
				}
			}
			return make_unique<queries::GetRigidBodiesResult>(bodies);
		}
		default:
			return make_unique<QueryResult>(QueryType::EMPTY);
		}
	}

	static string GetName() {
		return "PhysicsObjectCollection-" + ActorBase::GetName();
	}

private:
	void AddNewRigidBody(commands::AddRigidBody& args) {
		rigid_bodies_[args.name_] = PhysicsObjectComponent(std::move(args.rigid_body_), args.enabled_);
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void RemoveExistingRigidBody(commands::RemoveRigidBody& args) {
		rigid_bodies_.erase(args.name_);
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void ModifyRigidBody(commands::ModifyRigidBody& args) {
		bool changed = false;
		if (args.modify_all_) {
			for (auto& rigid_body : rigid_bodies_) {
				changed |= args.ApplyModifier(rigid_body.second);
			}
		} else {
			for (const string& name : args.names_to_modify_) {
				auto rigid_body_iter = rigid_bodies_.find(name);
				if (rigid_body_iter == rigid_bodies_.end()) {
					std::cout << "Tried to toggle a non-existant rigid body" << std::endl;
					continue;
				}
				changed |= args.ApplyModifier(rigid_body_iter->second);
			}
		}
		if (changed && updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void RigidBodyPoseUpdated() {
		for (const auto& named_rigid_body : rigid_bodies_) {
			poseable::PushNewPose(*this, 
				named_rigid_body.first,
				bullet::poses::GetPose(named_rigid_body.second.body_.GetTransform()));
		}
	}

	void HandleNewPose(commands::AcceptNewPose& args) {
		if (args.pose_source_ != GetNameVirt()) {
			auto rigid_body_iter = rigid_bodies_.find(args.name_);
			if (rigid_body_iter == rigid_bodies_.end()) {
				std::cout << "Attempting to set a new pose of a rigid body that doesn't exist." << std::endl;
				return;
			}
			rigid_body_iter->second.body_.SetTransform(bullet::poses::GetTransform(args.new_pose_));
		}
	}

  unique_ptr<queries::CheckCollisionResult> CheckCollision(const queries::CheckCollisionQuery& args) const {
    const bullet::CollisionObject& other_object = args.collision_object_;
    for (const pair<const string, PhysicsObjectComponent>& sub_component : rigid_bodies_) {
      const PhysicsObjectComponent& rigid_body = sub_component.second;
      if (rigid_body.enabled_ && rigid_body.body_.GetBody()->checkCollideWith(other_object.GetCollisionObject())) {
        return make_unique<queries::CheckCollisionResult>(true);
      }
    }
    return make_unique<queries::CheckCollisionResult>(false);
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

