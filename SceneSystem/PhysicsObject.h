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
	DECLARE_COMMAND(PhysicsObjectCommand, TOGGLE_RIGID_BODY);
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

struct ToggleRigidBody : public CommandArgs {
	ToggleRigidBody(bool enable) :
		CommandArgs(PhysicsObjectCommand::TOGGLE_RIGID_BODY), toggle_all_(true), names_to_toggle_() {}
	ToggleRigidBody(bool enable, string name) :
		CommandArgs(PhysicsObjectCommand::TOGGLE_RIGID_BODY), toggle_all_(false), names_to_toggle_({ name }) {}
	ToggleRigidBody(bool enable, vector<string> names) :
		CommandArgs(PhysicsObjectCommand::TOGGLE_RIGID_BODY), toggle_all_(false), names_to_toggle_(std::move(names)) {}

	bool enable_;
	bool toggle_all_;
	vector<string> names_to_toggle_;
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
		case commands::PhysicsObjectCommand::TOGGLE_RIGID_BODY:
			ToggleRigidBody(dynamic_cast<commands::AddRigidBody&>(args));
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
			if (enabled_) {
				return make_unique<queries::GetRigidBodiesResult>(rigid_body_);
			} else {
				return make_unique<queries::GetRigidBodiesResult>(vector<const bullet::RigidBody*>());
			}
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
		enabled_ = args.enabled_;
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

	void ToggleRigidBody(commands::ToggleRigidBody& args) {
		if (args.enable_ != enabled_) {
			enabled_ = args.enable_;
			if (updated_callback_) {
				updated_callback_(GetId());
			}
		}
	}

	void RigidBodyPoseUpdated() {
		poseable::PushNewPose(*this, std::make_pair(
			rigid_body_name_,
			bullet::poses::GetPose(rigid_body_.GetTransform())));
	}

	bool enabled_ = true;
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
		case commands::PhysicsObjectCommand::TOGGLE_RIGID_BODY:
			ToggleRigidBody(dynamic_cast<commands::AddRigidBody&>(args));
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
				if (rigid_body.second.second) {
					bodies.push_back(&rigid_body.second.first);
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
		rigid_bodies_[args.name_] = std::make_pair(std::move(args.rigid_body_), args.enabled_);
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

	void ToggleRigidBody(commands::ToggleRigidBody& args) {
		bool changed = false;
		if (args.toggle_all_) {
			for (auto& rigid_body : rigid_bodies_) {
				changed |= (rigid_body.second.second != args.enable_);
				rigid_body.second.second = args.enable_;
			}
		} else {
			for (const string& name : args.names_to_toggle_) {
				auto rigid_body_iter = rigid_bodies_.find(name);
				if (rigid_body_iter == rigid_bodies_.end()) {
					std::cout << "Tried to toggle a non-existant rigid body" << std::endl;
					continue;
				}
				auto& rigid_body = *rigid_body_iter;
				changed |= (rigid_body.second.second != args.enable_);
				rigid_body.second.second = args.enable_;
			}
		}
		if (changed && updated_callback_) {
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

	map<string, pair<bullet::RigidBody, bool>> rigid_bodies_;
	game_scene::ActorCallback updated_callback_;
};

template class PhysicsObject<Poseable<ActorImpl>>;
template class PhysicsObject<ActorImpl>;
template class PhysicsObjectCollection<Poseable<ActorImpl>>;
template class PhysicsObjectCollection<ActorImpl>;

}
}

