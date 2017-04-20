#pragma once

#include "stdafx.h"

#include "BulletInternals.h"

#include "CommandArgs.h"
#include "QueryArgs.h"
#include "QueryResult.h"
#include "PhysicsInteractions.h"
#include "Poseable.h"

namespace game_scene {
namespace commands {

class PhysicsObjectCommand {
public:
	DECLARE_COMMAND(PhysicsObjectCommand, ADD_RIGID_BODY);
	DECLARE_COMMAND(PhysicsObjectCommand, REMOVE_RIGID_BODY);
	DECLARE_COMMAND(PhysicsObjectCommand, ADD_UPDATED_CALLBACK);
};

struct AddRigidBody : public CommandArgs {
	AddRigidBody(string name, bullet::RigidBody rigid_body) :
		CommandArgs(PhysicsObjectCommand::ADD_RIGID_BODY),
		name_(name),
		rigid_body_(std::move(rigid_body)) {}
	AddRigidBody(bullet::RigidBody rigid_body) :
		AddRigidBody("", std::move(rigid_body)) {}

	string name_;
	bullet::RigidBody rigid_body_;
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

}  // commands

namespace queries {

class PhysicsObjectQuery {
public:
	DECLARE_QUERY(PhysicsObjectQuery, GET_RIGID_BODIES);
};

struct GetRigidBodies : QueryArgs {
	GetRigidBodies();
};

struct GetRigidBodiesResult : QueryResult {
	GetRigidBodiesResult(const bullet::RigidBody& rigid_body) :
		QueryResult(PhysicsObjectQuery::GET_RIGID_BODIES), rigid_bodies_() {
		rigid_bodies_.push_back(&rigid_body);
	}
	GetRigidBodiesResult(vector<const bullet::RigidBody*> rigid_bodies) :
		QueryResult(PhysicsObjectQuery::GET_RIGID_BODIES), rigid_bodies_(std::move(rigid_bodies)) {
	}

	vector<const bullet::RigidBody*> rigid_bodies_;
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
			return make_unique<queries::GetRigidBodiesResult>(rigid_body_);
		default:
			return make_unique<QueryResult>(QueryType::EMPTY);
		}
	}

	static string GetName() {
		return "PhysicsObject-" + ActorBase::GetName();
	}

private:
	void SetRigidBody(commands::AddRigidBody& args) {
		rigid_body_name_ = args.name_;
		rigid_body_ = std::move(args.rigid_body_);
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void ClearRigidBody() {
		rigid_body_ = bullet::RigidBody();
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void RigidBodyPoseUpdated() {
		poseable::PushNewPose(*this, std::make_pair(
			rigid_body_name_,
			bullet::poses::GetPose(rigid_body_.GetTransform())));
	}

	string rigid_body_name_;
	bullet::RigidBody rigid_body_;
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
			bodies.reserve(rigid_bodies_.size());
			for (const auto& rigid_body : rigid_bodies_) {
				bodies.push_back(&rigid_body.second);
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
		rigid_bodies_[args.name_] = std::move(args.rigid_body_);
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

	void RigidBodyPoseUpdated() {
		for (const auto& named_rigid_body : rigid_bodies_) {
			poseable::PushNewPose(*this, std::make_pair(
				named_rigid_body.first,
				bullet::poses::GetPose(named_rigid_body.second.GetTransform())));
		}
	}

	map<string, bullet::RigidBody> rigid_bodies_;
	game_scene::ActorCallback updated_callback_;
};

template class PhysicsObject<Poseable<ActorImpl>>;
template class PhysicsObject<ActorImpl>;
template class PhysicsObjectCollection<Poseable<ActorImpl>>;
template class PhysicsObjectCollection<ActorImpl>;

}
}

