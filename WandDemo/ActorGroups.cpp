#include "stdafx.h"
#include "ActorGroups.h"

#include <cassert>

namespace game_scene {
ActorGroups::ActorGroups() {
}

void ActorGroups::AddActorToGroup(ActorId actor, ActorId group) {
	FindMutableGroupSet(group).insert(actor);
	actor_to_groups_[actor].insert(group);
}

bool ActorGroups::IsActorInGroup(ActorId actor, ActorId group) const {
	const set<ActorId>& group_actors = FindGroupSet(group);
	return group_actors.find(actor) != group_actors.end();
}

void ActorGroups::RemoveActorFromGroup(ActorId actor, ActorId group) {
	set<ActorId>& group_actors = FindMutableGroupSet(group);
	assert(group_actors.find(actor) != group_actors.end());
	group_actors.erase(actor);
}

void ActorGroups::RemoveActorFromGroupBlindly(ActorId actor, ActorId group) {
	if (IsActorInGroup(actor, group)) {
		RemoveActorFromGroup(actor, group);
	}
}

void ActorGroups::RemoveActorFromAllGroups(ActorId actor) {
	for (const ActorId& group : actor_to_groups_[actor]) {
		set<ActorId>& group_actors = FindMutableGroupSet(group);
		assert(group_actors.find(actor) != group_actors.end());
		group_actors.erase(actor);
	}
	actor_to_groups_.erase(actor);
}

void ActorGroups::RemoveGroup(ActorId group) {
	for (const ActorId& actor : FindMutableGroupSet(group)) {
		actor_to_groups_[actor].erase(group);
	}
	group_to_actors_.erase(group);
}

void ActorGroups::CreateGroup(ActorId group) {
	group_to_actors_[group] = set<ActorId>();
}

bool ActorGroups::IsGroup(ActorId group) const {
	return group_to_actors_.find(group) != group_to_actors_.end();
}

const set<ActorId>& ActorGroups::FindGroupSet(ActorId group) const {
	auto group_actors = group_to_actors_.find(group);
	assert(group_actors != group_to_actors_.end());
	return group_actors->second;
}

set<ActorId>& ActorGroups::FindMutableGroupSet(ActorId group) {
	auto group_actors = group_to_actors_.find(group);
	assert(group_actors != group_to_actors_.end());
	return group_actors->second;
}

}  // game_scene
