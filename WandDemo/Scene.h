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
	CommandQueueLocation MakeCommandAfter(CommandQueueLocation location, const Target& target, unique_ptr<CommandArgs> args);
	CommandQueueLocation FrontOfCommands();
	unique_ptr<QueryResult> AskQuery(const Target& target, unique_ptr<QueryArgs> args);

	// Interface for main loop
	void FlushCommandQueue();
	void ExecuteCommand(const Command& command);

	// Universal interface
	ActorId AddActor(unique_ptr<Shmactor> new_actor);
	ActorId AddActorGroup();
	void AddActorToGroup(ActorId actor, ActorId group);

private:
	Shmactor* FindActor(const ActorId& actor_id);
	vector<ActorId> ExpandTarget(const Target& target);

	CommandQueue commands_;
	map<ActorId, unique_ptr<Shmactor>> actor_lookup_;
	ActorGroups actor_groups_;
	queue<ActorId> actors_to_be_deleted_;
	// The value depends on the existance of the key. Deleting the key requires alerting the value.
	multimap<ActorId, ActorId> actor_dependencies_;
};
}  // game_scene