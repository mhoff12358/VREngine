#pragma once

#include "stdafx.h"

#include "ActorGroups.h"
#include "ActorId.h"
#include "CommandQueue.h"
#include "QueryResult.h"

namespace game_scene {
class Shmactor;
class QueryArgs;

class Scene
{
public:
	Scene();
	~Scene();
	Scene(const Scene&) = delete;

	// Interface for Actors
	void RegisterDependency(const Target& depender, const Target& dependent);
	CommandQueueLocation MakeCommandAfter(CommandQueueLocation location, Command command);
	CommandQueueLocation FrontOfCommands();
	unique_ptr<QueryResult> AskQuery(const Target& target, const QueryArgs& args);
	void OwnCommandArgsForFlushDuration(CommandArgs* args_to_own);

	// Interface for main loop
	void FlushCommandQueue();
	void ExecuteCommand(const Command& command);

	// Universal interface
	ActorId AddActor(unique_ptr<Shmactor> new_actor, unique_ptr<CommandArgs> args = nullptr);
	ActorId AddActor(unique_ptr<Shmactor> new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args = nullptr);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(unique_ptr<Shmactor> new_actor, unique_ptr<CommandArgs> args = nullptr);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(unique_ptr<Shmactor> new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args = nullptr);
	ActorId AddActor(Shmactor& new_actor, unique_ptr<CommandArgs> args = nullptr);
	ActorId AddActor(Shmactor& new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args = nullptr);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(Shmactor& new_actor, unique_ptr<CommandArgs> args = nullptr);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(Shmactor& new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args = nullptr);
	ActorId AddActorGroup();
	void AddActorToGroup(ActorId actor, ActorId group);
	void RemoveActorFromGroup(ActorId actor, ActorId group);

	ActorId RegisterByName(string name, ActorId actor_or_group);
	ActorId FindByName(string name);

private:
	Shmactor* FindActor(const ActorId& actor_id);
	vector<ActorId> ExpandTarget(const Target& target);

	vector<unique_ptr<CommandArgs>> command_flush_arg_storage_;

	CommandQueue commands_;
	map<ActorId, Shmactor*> actor_lookup_;
	map<ActorId, unique_ptr<Shmactor>> actor_lookup_unique_;
	map<ActorId, Shmactor*> actor_lookup_non_unique_;
	ActorGroups actor_groups_;
	queue<ActorId> actors_to_be_deleted_;
	// The value depends on the existance of the key. Deleting the key requires alerting the value.
	multimap<ActorId, ActorId> actor_dependencies_;

	map<string, ActorId> registered_actors_or_groups_;

	int next_actor_id_;
	ActorId NextActorId() { return ActorId(next_actor_id_++); }
};
}  // game_scene