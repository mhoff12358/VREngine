#pragma once
#include "stdafx.h"

#include "Actor.h"
#include "CommandArgs.h"
#include "CollisionShape.h"
#include "HeadsetInterface.h"

namespace game_scene {
class CollisionCollectionCommand {
public:
DECLARE_COMMAND(CollisionCollectionCommand, ADD_COLLIDEABLE_OBJECT)
DECLARE_COMMAND(CollisionCollectionCommand, REPOSE_COLLIDEABLE_OBJECT)
DECLARE_COMMAND(CollisionCollectionCommand, ENDISABLE_COLLIDEABLE_OBJECT)
DECLARE_COMMAND(CollisionCollectionCommand, REMOVE_COLLIDEABLE_OBJECT)

DECLARE_COMMAND(CollisionCollectionCommand, INTERNAL_COLLISION)
};

class AddCollideableObject : public CommandArgs {
public:
	AddCollideableObject(ActorId grabbable_actor, vector<CollisionShape>& grab_shapes) :
		CommandArgs(CollisionCollectionCommand::ADD_COLLIDEABLE_OBJECT),
		grabbable_actor_(grabbable_actor),
		grab_shapes_(std::move(grab_shapes)) {}

	ActorId grabbable_actor_;
	vector<CollisionShape> grab_shapes_;
};

class ReposeCollideableObject : public CommandArgs {
public:
	ReposeCollideableObject(ActorId grabbable_actor, Pose pose, int shape_index = -1) :
		CommandArgs(CollisionCollectionCommand::REPOSE_COLLIDEABLE_OBJECT),
		grabbable_actor_(grabbable_actor), pose_(pose), shape_index_(shape_index) {}

	ActorId grabbable_actor_;
	Pose pose_;
	int shape_index_;
};

class EnDisableCollideableObject : public CommandArgs {
public:
	EnDisableCollideableObject(ActorId grabbable_actor, bool enable, int shape_index = -1) :
		CommandArgs(CollisionCollectionCommand::ENDISABLE_COLLIDEABLE_OBJECT),
		grabbable_actor_(grabbable_actor), enable_(enable), shape_index_(shape_index) {}

	ActorId grabbable_actor_;
	bool enable_;
	int shape_index_;
};

class RemoveCollideableObject : public CommandArgs {
public:
	RemoveCollideableObject(ActorId grabbable_actor)
		: CommandArgs(CollisionCollectionCommand::REMOVE_COLLIDEABLE_OBJECT), grabbable_actor_(grabbable_actor) {}

	ActorId grabbable_actor_;
};

class InternalCollision : public CommandArgs {
public:
	InternalCollision(ActorId instigating_actor, set<ActorId> additional_collisions) :
		CommandArgs(CollisionCollectionCommand::INTERNAL_COLLISION),
		additional_collisions_(std::move(additional_collisions)) {}

	void PassOn(Scene& scene, CommandQueueLocation location);

	ActorId instigating_actor_;
	set<ActorId> additional_collisions_;
};

class CollisionCollectionImpl : public ActorImpl {
public:
	CollisionCollectionImpl();

	void HandleCommand(const CommandArgs& args);
	void HandleAddCollideableObject(const AddCollideableObject& args);
	void HandleReposeCollideableObject(const ReposeCollideableObject& args);
	void HandleEnDisableCollideableObject(const EnDisableCollideableObject& args);
	void HandleRemoveCollideableObject(const RemoveCollideableObject& args);

	string GetName() const {
		return "CollisionCollectionImpl-" + ActorImpl::GetName();
	}

protected:
	template <typename CollisionTest>
	ActorId FindCollidingActor(const CollisionTest& test_shape, ActorId ignored_actor = ActorId::UnsetId);
	template <typename CollisionTest>
	set<ActorId> FindCollidingActors(const CollisionTest& test_shape, ActorId ignored_actor = ActorId::UnsetId);
	vector<CollisionShape>& GetCollisionShapes(ActorId actor_id);

private:
	void AddCollisionInformation(ActorId actor_id, vector<CollisionShape> collision);
	void RemoveCollisionInformation(ActorId actor_id);

	vector<pair<ActorId, vector<CollisionShape>>> actor_collisions_;
	set<ActorId> collideable_actors_;
};
ADD_ACTOR_ADAPTER(CollisionCollection);

template <typename CollisionTest>
ActorId CollisionCollection::FindCollidingActor(const CollisionTest& test_shape, ActorId ignored_actor) {
	for (const pair<ActorId, vector<CollisionShape>>& collidable_actor : actor_collisions_) {
		if (collidable_actor.first != ignored_actor) {
			for (const CollisionShape& actor_shape : collidable_actor.second) {
				if (actor_shape.Intersect(test_shape)) {
					return collidable_actor.first;
				}
			}
		}
	}
	return ActorId::UnsetId;
}

template <typename CollisionTest>
set<ActorId> CollisionCollection::FindCollidingActors(const CollisionTest& test_shape, ActorId ignored_actor) {
	set<ActorId> collided_actors;
	for (const pair<ActorId, vector<CollisionShape>>& collidable_actor : actor_collisions_) {
		if (collidable_actor.first != ignored_actor) {
			for (const CollisionShape& actor_shape : collidable_actor.second) {
				if (actor_shape.Intersect(test_shape)) {
					collided_actors.insert(collidable_actor.first);
				}
			}
		}
	}
	return collided_actors;
}

class CollisionCollectionCheckInternalCollisionsImpl : public CollisionCollection {
public:
	void HandleCommand(const CommandArgs& args);
	void HandleAddCollideableObject(const AddCollideableObject& args);
	void HandleReposeCollideableObject(const ReposeCollideableObject& args);

	void HandleInternalCollision(ActorId instigating_actor, const set<ActorId>& collided_actors);

	string GetName() const {
		return "CollisionCollectionCheckInternalCollisionsImpl-" + ActorImpl::GetName();
	}
};
ADD_ACTOR_ADAPTER(CollisionCollectionCheckInternalCollisions);
}
