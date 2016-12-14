#pragma once
#include "stdafx.h"

#include "Actor.h"
#include "CommandArgs.h"
#include "CollisionShape.h"
#include "HeadsetInterface.h"
#include "CollisionCollection.h"

namespace game_scene {
namespace commands {
class TriggerStateChange;
}  // commands

class GrabbableObjectCommand {
public:
DECLARE_COMMAND(GrabbableObjectCommand, DROP_GRABBABLE_OBJECT)
DECLARE_COMMAND(GrabbableObjectCommand, OBJECT_GRABBED)
};

class DropGrabbableObject : public CommandArgs {
public:
	DropGrabbableObject(ActorId grabbable_actor) :
		CommandArgs(GrabbableObjectCommand::DROP_GRABBABLE_OBJECT), grabbable_actor_(grabbable_actor) {}

	ActorId grabbable_actor_;
};

class ObjectGrabbed : public commands::ControllerInformation {
public:
	ObjectGrabbed(bool held, unsigned char controller_number, Pose controller_position = Pose()) :
		commands::ControllerInformation(
			GrabbableObjectCommand::OBJECT_GRABBED,
			controller_number,
			controller_position),
		held_(held) {}

	bool held_;
};

class GrabbableObjectHandler : public CollisionCollection {
public:
	GrabbableObjectHandler();

	void HandleCommand(const CommandArgs& args) override;
	void HandleDropGrabbableObject(const DropGrabbableObject& args);
	void HandleTriggerChange(const commands::TriggerStateChange& args);

	void DropActor(ActorId actor_id);
	void DropController(unsigned char controller_number);

	void SetGrabbedActor(unsigned char controller_number, ActorId grabbed_actor);
	void UnsetGrabbedActor(unsigned char controller_number);

private:
	array<ActorId, 2> grabbed_actor_ = { ActorId::UnsetId, ActorId::UnsetId };
};
}