#pragma once

#include "stdafx.h"

#include "BulletInternals.h"

#include "CommandArgs.h"
#include "QueryArgs.h"
#include "QueryResult.h"

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

struct AddUpdatedCallback : public CommandType {
	AddUpdatedCallback(game_scene::ActorCallback callback)
		: CommandType(PhysicsObjectCommand::ADD_UPDATED_CALLBACK), callback_(callback) {}

	game_scene::ActorCallback callback_;
};

}  // commands

namespace queries {

class PhysicsObjectQuery {
public:
	DECLARE_QUERY(PhysicsObjectQuery, GET_RIGID_BODIES);
};

struct GetRigidBodies : QueryArgs {
	GetRigidBodies() : QueryArgs(PhysicsObjectQuery::GET_RIGID_BODIES) {}
};

struct GetRigidBodiesResult : QueryResult {
	GetRigidBodiesResult(const bullet::RigidBody& rigid_body) :
		QueryResult(PhysicsObjectQuery::GET_RIGID_BODIES), rigid_body_(rigid_body) {}

	const bullet::RigidBody& rigid_body_;
};

}  // queries

namespace actors {

template <typename ActorBase>
class PhysicsObject : public ActorBase {
public:
	PhysicsObject() : ActorBase(), rigid_body_(nullptr), world_() {}

	void HandleCommand(CommandArgs& args) {
		switch (args.Type()) {
		case ADD_RIGID_BODY:
			SetRigidBody(std::move(dynamic_cast<AddRigidBody&>(args).rigid_body_));
			break;
		case REMOVE_RIGID_BODY:
			ClearRigidBody();
			break;
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) const {
		switch (args.Type()) {
		case PhysicsObjectQuery::GET_RIGID_BODIES:
			return make_unique<GetRigidBodiesResult>(rigid_body_);
		default:
			return make_unqiue<QueryResult>(QueryType::EMPTY);
		}
	}

	static string GetName() {
		return "PhysicsObject-" + ActorBase::GetName();
	}

private:
	void SetRigidBody(bullet::RigidBody rigid_body) {
		rigid_body_ = std::move(rigid_body);
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void ClearRigidBody() {
		rigid_body_ = RigidBody();
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	bullet::RigidBody rigid_body_;
	game_scene::ActorCallback updated_callback_;
};

template <typename ActorBase>
class PhysicsObjectCollection : public ActorBase {
public:
	PhysicsObjectCollection() : ActorBase(), rigid_body_(nullptr), world_() {}

	void HandleCommand(CommandArgs& args) {
		switch (args.Type()) {
		case ADD_RIGID_BODY:
			AddNewRigidBody(dynamic_cast<commands::AddRigidBody&>(args));
			break;
		case REMOVE_RIGID_BODY:
			RemoveExistingRigidBody(dynamic_cast<commands::RemoveRigidBody&>(args));
			break;
		default:
			break;
		}
		ActorBase::HandleCommand(args);
	}

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) const {
		switch (args.Type()) {
		case PhysicsObjectQuery::GET_RIGID_BODIES:
			return make_unique<queries::GetRigidBodiesResult>(rigid_body_);
		default:
			return make_unqiue<QueryResult>(QueryType::EMPTY);
		}
	}

	static string GetName() {
		return "PhysicsObjectCollection-" + ActorBase::GetName();
	}

private:
	void AddNewRigidBody(commands::AddRigidBody args) {
		rigid_bodies_[args.name_] = std::move(args.rigid_body_);
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	void RemoveExistingRigidBody(commands::RemoveRigidBody args) {
		rigid_bodies_.erase(args.name_);
		if (updated_callback_) {
			updated_callback_(GetId());
		}
	}

	map<string, bullet::RigidBody> rigid_bodies_;
	game_scene::ActorCallback updated_callback_;
};

}
}
