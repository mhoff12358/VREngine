#include "stdafx.h"
#include "CollisionCollection.h"

#include "Scene.h"

namespace game_scene {
void InternalCollision::PassOn(Scene& scene, CommandQueueLocation location) {
	if (!additional_collisions_.empty()) {
		ActorId new_collided_actor = *additional_collisions_.cbegin();
		set<ActorId> remaining_additional_collisions(additional_collisions_.cbegin()++, additional_collisions_.cend());
		scene.MakeCommandAfter(
			location,
			Command(
				Target(new_collided_actor),
				make_unique<InternalCollision>(instigating_actor_, remaining_additional_collisions)));
	}
}

CollisionCollectionImpl::CollisionCollectionImpl() {

}

void CollisionCollectionImpl::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CollisionCollectionCommand::ADD_COLLIDEABLE_OBJECT:
		HandleAddCollideableObject(dynamic_cast<const AddCollideableObject&>(args));
		break;
	case CollisionCollectionCommand::REMOVE_COLLIDEABLE_OBJECT:
		HandleRemoveCollideableObject(dynamic_cast<const RemoveCollideableObject&>(args));
		break;
	case CollisionCollectionCommand::ENDISABLE_COLLIDEABLE_OBJECT:
		HandleEnDisableCollideableObject(dynamic_cast<const EnDisableCollideableObject&>(args));
		break;
	case CollisionCollectionCommand::REPOSE_COLLIDEABLE_OBJECT:
		HandleReposeCollideableObject(dynamic_cast<const ReposeCollideableObject&>(args));
		break;
	}
}

void CollisionCollectionImpl::HandleAddCollideableObject(const AddCollideableObject& args) {
	// If this actor already has a held location then remove it first.
	if (collideable_actors_.find(args.grabbable_actor_) != collideable_actors_.end()) {
		RemoveCollisionInformation(args.grabbable_actor_);
	}
	AddCollisionInformation(args.grabbable_actor_, args.grab_shapes_);
}

void CollisionCollectionImpl::HandleReposeCollideableObject(const ReposeCollideableObject& args) {
	vector<CollisionShape>& collision_shapes = GetCollisionShapes(args.grabbable_actor_);
	if (args.shape_index_ == -1) {
		for (CollisionShape& shape : collision_shapes) {
			shape.SetPose(args.pose_);
		}
	} else {
		collision_shapes[args.shape_index_].SetPose(args.pose_);
	}
}

void CollisionCollectionImpl::HandleEnDisableCollideableObject(const EnDisableCollideableObject& args) {
	vector<CollisionShape>& collision_shapes = GetCollisionShapes(args.grabbable_actor_);
	if (args.shape_index_ == -1) {
		for (CollisionShape& shape : collision_shapes) {
			shape.EnDisable(args.enable_);
		}
	} else {
		collision_shapes[args.shape_index_].EnDisable(args.enable_);
	}
}

void CollisionCollectionImpl::HandleRemoveCollideableObject(const RemoveCollideableObject& args) {
	RemoveCollisionInformation(args.grabbable_actor_);
}

void CollisionCollectionImpl::AddCollisionInformation(ActorId actor_id, vector<CollisionShape> collision) {
	actor_collisions_.emplace_back(make_pair(actor_id, std::move(collision)));
	collideable_actors_.insert(actor_id);
}

void CollisionCollectionImpl::RemoveCollisionInformation(ActorId actor_id) {
	unsigned int remove_index = 0;
	while (
		(actor_id != actor_collisions_[remove_index].first) &&
		(remove_index != actor_collisions_.size())) {
		remove_index++;
	}
	if (remove_index == actor_collisions_.size()) {
		return;
	}
	// It's safe to grab the back without checking the size because if it were empty
	// then remove_index would have been always 0 and the previous check would have
	// returned
	actor_collisions_[remove_index] = std::move(actor_collisions_.back());
	actor_collisions_.pop_back();
	collideable_actors_.erase(actor_id);
}

vector<CollisionShape>& CollisionCollectionImpl::GetCollisionShapes(ActorId actor_id) {
	size_t index = 0;
	while (
		(index != actor_collisions_.size()) &&
		(actor_id != actor_collisions_[index].first)) {
		index++;
	}
	if (index == actor_collisions_.size()) {
		actor_collisions_.emplace_back();
	}
	return actor_collisions_[index].second;
}

void CollisionCollectionCheckInternalCollisionsImpl::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CollisionCollectionCommand::ADD_COLLIDEABLE_OBJECT:
		HandleAddCollideableObject(dynamic_cast<const AddCollideableObject&>(args));
		break;
	case CollisionCollectionCommand::REPOSE_COLLIDEABLE_OBJECT:
		HandleReposeCollideableObject(dynamic_cast<const ReposeCollideableObject&>(args));
		break;
	default:
		CollisionCollectionImpl::HandleCommand(args);
	}
}

void CollisionCollectionCheckInternalCollisionsImpl::HandleAddCollideableObject(const AddCollideableObject& args) {
	CollisionCollectionImpl::HandleAddCollideableObject(args);
	set<ActorId> colliding_actors = FindCollidingActors(args.grab_shapes_, args.grabbable_actor_);
	HandleInternalCollision(args.grabbable_actor_, colliding_actors);
}

void CollisionCollectionCheckInternalCollisionsImpl::HandleReposeCollideableObject(const ReposeCollideableObject& args) {
	CollisionCollectionImpl::HandleReposeCollideableObject(args);
	set<ActorId> colliding_actors = FindCollidingActors(GetCollisionShapes(args.grabbable_actor_), args.grabbable_actor_);
	HandleInternalCollision(args.grabbable_actor_, colliding_actors);
}

void CollisionCollectionCheckInternalCollisionsImpl::HandleInternalCollision(ActorId instigating_actor, const set<ActorId>& collided_actors) {
	ActorId first_collided_actor = *collided_actors.cbegin();
	set<ActorId> additional_collisions(collided_actors.cbegin()++, collided_actors.cend());
	GetScene().MakeCommandAfter(
		GetScene().BackOfNewCommands(),
		Command(
			Target(first_collided_actor),
			make_unique<InternalCollision>(instigating_actor, additional_collisions)));
}
}
