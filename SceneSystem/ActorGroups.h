#pragma once

#include "stdafx.h"
#include "ActorId.h"

namespace game_scene {
class DLLSTUFF ActorGroups {
public:
	ActorGroups();

	void AddActorToGroup(ActorId actor, ActorId group);
	bool IsActorInGroup(ActorId actor, ActorId group) const;
	void RemoveActorFromGroup(ActorId actor, ActorId group);
	void RemoveActorFromGroupBlindly(ActorId actor, ActorId group);
	void RemoveActorFromAllGroups(ActorId actor);
	void RemoveGroup(ActorId group);
	void CreateGroup(ActorId group);
	bool IsGroup(ActorId group) const;
	const set<ActorId>& FindGroupSet(ActorId group) const;

private:
	map<ActorId, set<ActorId>> group_to_actors_;
	map<ActorId, set<ActorId>> actor_to_groups_;

	set<ActorId>& FindMutableGroupSet(ActorId group);
};
}  // game_scene