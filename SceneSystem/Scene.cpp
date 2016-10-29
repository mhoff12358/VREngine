#include "stdafx.h"
#include "Scene.h"
#include "Target.h"
#include "Shmactor.h"
#include "CommandArgs.h"

namespace game_scene {

Scene::Scene() : next_actor_id_(ActorId::FirstId) {
	actor_lookup_non_unique_[ActorId(5)] = nullptr;
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

CommandQueueLocation Scene::MakeCommandAfter(CommandQueueLocation location, Command command) {
	return commands_.InsertCommand(location, move(command));
}

CommandQueueLocation Scene::FrontOfCommands() {
	return commands_.GetNewFront();
}

unique_ptr<QueryResult> Scene::AskQuery(const Target& target, const QueryArgs& args) {
	vector<ActorId> actors_to_query = ExpandTarget(target);
	if (actors_to_query.size() == 0) {
		return make_unique<QueryResult>(QueryType::EMPTY);
	}
	if (actors_to_query.size() == 1) {
		return FindActor(actors_to_query[0])->AnswerQuery(args);
	}
	unique_ptr<MultipleQueryResult> query_results = make_unique<MultipleQueryResult>();
	for (const ActorId& query_id : ExpandTarget(target)) {
		query_results->AddResult(query_id, move(FindActor(query_id)->AnswerQuery(args)));
	}
	return move(query_results);
}

void Scene::OwnCommandArgsForFlushDuration(CommandArgs* args_to_own) {
	command_flush_arg_storage_.emplace_back(args_to_own);
}

void Scene::FlushCommandQueue() {
	while (!commands_.IsEmpty()) {
		Command active_command = commands_.PopActiveCommand();
		for (const ActorId& actor_id : ExpandTarget(active_command.GetTarget())) {
			FindActor(actor_id)->HandleCommand(active_command.GetArgs());
		}
	}
	command_flush_arg_storage_.clear();
}

void Scene::ExecuteCommand(const Command& command) {
	for (const ActorId& actor_id : ExpandTarget(command.GetTarget())) {
		FindActor(actor_id)->HandleCommand(command.GetArgs());
	}
	FlushCommandQueue();
}

Shmactor* Scene::FindActor(const ActorId& actor_id) {
	auto actor_iter = actor_lookup_.find(actor_id);
	if (actor_iter == actor_lookup_.end()) {
		return nullptr;
	}
	return actor_lookup_[actor_id].get();
}

vector<ActorId> Scene::ExpandTarget(const Target& target) {
	vector<ActorId> actors_to_return;
	switch (target.actor_id_.id_) {
	case ALL_ACTORS:
		actors_to_return = {};
		break;
	case UNSET_ID:
		actors_to_return = {};
		break;
	default:
		if (actor_groups_.IsGroup(target.actor_id_)) {
			const set<ActorId> actor_set = actor_groups_.FindGroupSet(target.actor_id_);
			actors_to_return = vector<ActorId>(actor_set.begin(), actor_set.end());
		} else {
			actors_to_return = {{target.actor_id_}};
		}
		break;
	}
	return actors_to_return;
}

ActorId Scene::AddActor(unique_ptr<Shmactor> new_actor, unique_ptr<CommandArgs> args) {
	return get<0>(AddActorReturnInitialize(move(new_actor), move(args)));
}

ActorId Scene::AddActor(unique_ptr<Shmactor> new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args) {
	return get<0>(AddActorReturnInitialize(move(new_actor), initialize_after, move(args)));
}

tuple<ActorId, CommandQueueLocation> Scene::AddActorReturnInitialize(unique_ptr<Shmactor> new_actor, unique_ptr<CommandArgs> args) {
	return AddActorReturnInitialize(move(new_actor), FrontOfCommands(), move(args));
}

tuple<ActorId, CommandQueueLocation> Scene::AddActorReturnInitialize(unique_ptr<Shmactor> new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args) {
	ActorId new_id = NextActorId();
	new_actor->SetId(new_id);
	new_actor->SetScene(this);
	actor_lookup_.emplace(new_id, move(new_actor));
	actor_lookup_[new_id]->AddedToScene();
	if (args == nullptr) {
		args = make_unique<CommandArgs>(CommandType::ADDED_TO_SCENE);
	}
	assert(args->Type() == CommandType::ADDED_TO_SCENE);
	return tuple<ActorId, CommandQueueLocation>(new_id, MakeCommandAfter(initialize_after, Command(Target(new_id), move(args))));
}

ActorId Scene::AddActorGroup() {
	ActorId new_id = NextActorId();
	actor_groups_.CreateGroup(new_id);
	return new_id;
}

void Scene::AddActorToGroup(ActorId actor, ActorId group) {
	if (actor != ActorId::UnsetId && group != ActorId::UnsetId) {
		actor_groups_.AddActorToGroup(actor, group);
	}
}

void Scene::RemoveActorFromGroup(ActorId actor, ActorId group) {
	if (actor != ActorId::UnsetId && group != ActorId::UnsetId) {
		actor_groups_.RemoveActorFromGroup(actor, group);
	}
}

ActorId Scene::RegisterByName(string name, ActorId actor_or_group) {
	if (actor_or_group != ActorId::UnsetId) {
		registered_actors_or_groups_[name] = actor_or_group;
	}
	return actor_or_group;
}

ActorId Scene::FindByName(string name) {
	auto lookup = registered_actors_or_groups_.find(name);
	if (lookup == registered_actors_or_groups_.end()) {
		return ActorId(ActorId::UnsetId);
	}
	return lookup->second;
}

}  // game_scene