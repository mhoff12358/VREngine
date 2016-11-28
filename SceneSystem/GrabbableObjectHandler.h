#pragma once
#include "stdafx.h"

#include "Actor.h"
#include "CommandArgs.h"
#include "CollisionShape.h"
#include "HeadsetInterface.h"

namespace game_scene {
namespace commands {
class TriggerStateChange;
}  // commands

class GrabbableObjectCommand {
public:
DECLARE_COMMAND(GrabbableObjectCommand, ADD_GRABBABLE_OBJECT)
DECLARE_COMMAND(GrabbableObjectCommand, REPOSE_GRABBABLE_OBJECT)
DECLARE_COMMAND(GrabbableObjectCommand, ENDISABLE_GRABBABLE_OBJECT)
DECLARE_COMMAND(GrabbableObjectCommand, REMOVE_GRABBABLE_OBJECT)
DECLARE_COMMAND(GrabbableObjectCommand, OBJECT_GRABBED)
};

class AddGrabbableObject : public CommandArgs {
public:
	AddGrabbableObject(ActorId grabbable_actor, vector<CollisionShape>& grab_shapes) :
		CommandArgs(GrabbableObjectCommand::ADD_GRABBABLE_OBJECT),
		grabbable_actor_(grabbable_actor),
		grab_shapes_(std::move(grab_shapes)) {}

	ActorId grabbable_actor_;
	vector<CollisionShape> grab_shapes_;
};

class ReposeGrabbableObject : public CommandArgs {
public:
	ReposeGrabbableObject(ActorId grabbable_actor, Pose pose, int shape_index = -1) :
		CommandArgs(GrabbableObjectCommand::REPOSE_GRABBABLE_OBJECT),
		grabbable_actor_(grabbable_actor), pose_(pose), shape_index_(shape_index) {}

	ActorId grabbable_actor_;
	Pose pose_;
	int shape_index_;
};

class EnDisableGrabbableObject : public CommandArgs {
public:
	EnDisableGrabbableObject(ActorId grabbable_actor, bool enable, int shape_index = -1) :
		CommandArgs(GrabbableObjectCommand::ENDISABLE_GRABBABLE_OBJECT),
		grabbable_actor_(grabbable_actor), enable_(enable), shape_index_(shape_index) {}

	ActorId grabbable_actor_;
	bool enable_;
	int shape_index_;
};

class RemoveGrabbableObject : public CommandArgs {
public:
	RemoveGrabbableObject(ActorId grabbable_actor)
		: CommandArgs(GrabbableObjectCommand::REMOVE_GRABBABLE_OBJECT), grabbable_actor_(grabbable_actor) {}

	ActorId grabbable_actor_;
};

class ObjectGrabbed : public commands::ControllerInformation {
public:
	ObjectGrabbed(bool held, unsigned char controller_number, Pose controller_position) :
		commands::ControllerInformation(
			GrabbableObjectCommand::OBJECT_GRABBED,
			controller_number,
			controller_position),
		held_(held) {}

	bool held_;
};

class GrabbableObjectHandler : public Actor {
public:
	GrabbableObjectHandler();

	void HandleCommand(const CommandArgs& args) override;
	void HandleAddGrabbableObject(const AddGrabbableObject& args);
	void HandleReposeGrabbableObject(const ReposeGrabbableObject& args);
	void HandleEnDisableGrabbableObject(const EnDisableGrabbableObject& args);
	void HandleRemoveGrabbableObject(const RemoveGrabbableObject& args);
	void HandleTriggerChange(const commands::TriggerStateChange& args);

	ActorId FindCollidingActor(const CollisionShape& controller_shape);
	void SetGrabbedActor(unsigned char controller_number, ActorId grabbed_actor);
	void UnsetGrabbedActor(unsigned char controller_number);
	void AddGrabInformation(ActorId actor_id, vector<CollisionShape> collision);
	void RemoveGrabInformation(ActorId actor_id);

private:
	vector<CollisionShape>& GetCollisionShapes(ActorId actor_id);

	vector<pair<ActorId, vector<CollisionShape>>> grab_collisions_;
	set<ActorId> grabbable_actors_;
	array<ActorId, 2> grabbed_actor_ = { ActorId::UnsetId, ActorId::UnsetId };
};
}