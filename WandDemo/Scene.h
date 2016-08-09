#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "ActorId.h"
#include "CommandQueue.h"
#include "ActorGroups.h"

namespace game_scene {
class Shmactor;

class Scene
{
public:
	Scene();
	~Scene();

	// Interface for Actors
	void RegisterDependency(const Target& depender, const Target& dependent);
	CommandQueueLocation MakeCommandAfter(CommandQueueLocation location, Command command);
	CommandQueueLocation FrontOfCommands();
	unique_ptr<QueryResult> AskQuery(const Target& target, unique_ptr<QueryArgs> args);
	void OwnCommandArgsForFlushDuration(CommandArgs* args_to_own);

	// Interface for main loop
	void FlushCommandQueue();
	void ExecuteCommand(const Command& command);

	// Universal interface
	ActorId AddActor(unique_ptr<Shmactor> new_actor);
	ActorId AddActor(unique_ptr<Shmactor> new_actor, CommandQueueLocation initialize_after);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(unique_ptr<Shmactor> new_actor);
	tuple<ActorId, CommandQueueLocation> AddActorReturnInitialize(unique_ptr<Shmactor> new_actor, CommandQueueLocation initialize_after);
	ActorId AddActorGroup();
	void AddActorToGroup(ActorId actor, ActorId group);

	void RegisterByName(string name, ActorId actor_or_group);
	ActorId FindByName(string name);

private:
	Shmactor* FindActor(const ActorId& actor_id);
	vector<ActorId> ExpandTarget(const Target& target);

	vector<unique_ptr<CommandArgs>> command_flush_arg_storage_;

	CommandQueue commands_;
	map<ActorId, unique_ptr<Shmactor>> actor_lookup_;
	ActorGroups actor_groups_;
	queue<ActorId> actors_to_be_deleted_;
	// The value depends on the existance of the key. Deleting the key requires alerting the value.
	multimap<ActorId, ActorId> actor_dependencies_;

	map<string, ActorId> registered_actors_or_groups_;
};
}  // game_scene