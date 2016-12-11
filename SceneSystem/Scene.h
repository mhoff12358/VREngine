#pragma once

#include "stdafx.h"

#include "ActorGroups.h"
#include "ActorId.h"
#include "CommandQueue.h"
#include "QueryResult.h"

namespace game_scene {
class Actor;
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
	CommandQueueLocation BackOfNewCommands();
	unique_ptr<QueryResult> AskQuery(const Target& target, const QueryArgs& args);
	void OwnCommandArgsForFlushDuration(CommandArgs* args_to_own);

	// Interface for main loop
	void FlushCommandQueue();
	void ExecuteCommand(const Command& command);

	// Universal interface
	ActorId AddActor(unique_ptr<Actor> new_actor, unique_ptr<CommandArgs> args = nullptr);
	ActorId AddActor(unique_ptr<Actor> new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args = nullptr);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(unique_ptr<Actor> new_actor, unique_ptr<CommandArgs> args = nullptr);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(unique_ptr<Actor> new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args = nullptr);
	ActorId AddActor(Actor& new_actor, unique_ptr<CommandArgs> args = nullptr);
	ActorId AddActor(Actor& new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args = nullptr);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(Actor& new_actor, unique_ptr<CommandArgs> args = nullptr);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(Actor& new_actor, CommandQueueLocation initialize_after, unique_ptr<CommandArgs> args = nullptr);
	ActorId AddActorGroup();
	void AddActorToGroup(ActorId actor, ActorId group);
	void RemoveActorFromGroup(ActorId actor, ActorId group);

	ActorId RegisterByName(string name, ActorId actor_or_group);
	ActorId FindByName(string name);

private:
	Actor* FindActor(const ActorId& actor_id);
	vector<ActorId> ExpandTarget(const Target& target);

	vector<unique_ptr<CommandArgs>> command_flush_arg_storage_;

	CommandQueue commands_;
	CommandQueueLocation back_of_current_commands_;
	map<ActorId, unique_ptr<Actor>> actor_lookup_unique_;
	map<ActorId, Actor*> actor_lookup_non_unique_;
	map<ActorId, Actor*> actor_lookup_;
	ActorGroups actor_groups_;
	queue<ActorId> actors_to_be_deleted_;
	// The value depends on the existance of the key. Deleting the key requires alerting the value.
	multimap<ActorId, ActorId> actor_dependencies_;

	map<string, ActorId> registered_actors_or_groups_;

	int next_actor_id_;
	ActorId NextActorId() { return ActorId(next_actor_id_++); }
};
}  // game_scene