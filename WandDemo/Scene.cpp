#include "stdafx.h"
#include "Scene.h"
#include "Target.h"

namespace game_scene {

Scene::Scene() {
}

Scene::~Scene() {
}

vector<ActorId> ExpandTarget(const Target& target) {
	return {{target.actor_id_}};
}

void Scene::RegisterDependency(const Target& depender, const Target& dependent) {
	for (const ActorId& depender_id : ExpandTarget(depender)) {
		for (const ActorId& dependent_id : ExpandTarget(dependent)) {
			actor_dependencies_.insert(pair<ActorId, ActorId>(dependent_id, depender_id));
		}
	}
}

CommandQueueLocation Scene::MakeCommandAfter(CommandQueueLocation location, const Target& target, unique_ptr<CommandArgs> args) {
	return commands_.InsertCommand(location, move(Command(target, move(args))));
}

CommandQueueLocation Scene::FrontOfCommands() {
	return commands_.GetNewFront();
}

unique_ptr<QueryResult> Scene::AskQuery(const Target& target, unique_ptr<QueryArgs> args) {
	unique_ptr<MultipleQueryResult> query_results = make_unique<MultipleQueryResult>();
	for (const ActorId& query_id : ExpandTarget(target)) {
		query_results->AddResult(query_id, move(FindActor(query_id)->AnswerQuery(*args)));
	}
	return move(query_results);
}

void Scene::FlushCommandQueue() {
	while (!commands_.IsEmpty()) {
		Command active_command = commands_.PopActiveCommand();
		ExecuteCommand(active_command);
	}
}

void Scene::ExecuteCommand(const Command& command) {
	for (const ActorId& actor_id : ExpandTarget(command.GetTarget())) {
		FindActor(actor_id)->HandleCommand(command.GetArgs());
	}
}

Shmactor* Scene::FindActor(const ActorId& actor_id) {
	return actor_lookup_[actor_id].get();
}

vector<ActorId> Scene::ExpandTarget(const Target& target) {
	switch (target.actor_id_.id_) {
	case ALL_ACTORS:
		return {{}};
	case UNSET_ID:
		return {{}};
		break;
	default:
		if (actor_groups_.IsGroup(target.actor_id_)) {
			const set<ActorId> actor_set = actor_groups_.FindGroupSet(target.actor_id_);
			return vector<ActorId>(actor_set.begin(), actor_set.end());
		} else {
			return {{target.actor_id_}};
		}
	}
}

ActorId Scene::AddActor(unique_ptr<Shmactor> new_actor) {
	ActorId new_id = ActorId::GetNewId();
	new_actor->SetId(new_id);
	actor_lookup_.emplace(new_id, move(new_actor));
	return new_id;
}

ActorId Scene::AddActorGroup() {
	ActorId new_id = ActorId::GetNewId();
	actor_groups_.CreateGroup(new_id);
	return new_id;
}

void Scene::AddActorToGroup(ActorId actor, ActorId group) {
	actor_groups_.AddActorToGroup(actor, group);
}

}  // game_scene