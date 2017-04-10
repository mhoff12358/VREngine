#include "stdafx.h"
#include "GrabbableObjectHandler.h"

#include "HeadsetInterface.h"
#include "Scene.h"

namespace game_scene {
GrabbableObjectHandler::GrabbableObjectHandler() {

}

void GrabbableObjectHandler::HandleCommand(CommandArgs& args) {
	switch (args.Type()) {
	case CommandType::ADDED_TO_SCENE:
		GetScene().MakeCommandAfter(
			GetScene().FrontOfCommands(),
			Command(
				Target(GetScene().FindByName("HeadsetInterface")),
				make_unique<commands::HeadsetListenerRegistration>(
					true, GetId(), actors::HeadsetInterface::ListenerId::TRIGGER_STATE_CHANGE)));
		break;
	case GrabbableObjectCommand::DROP_GRABBABLE_OBJECT:
		HandleDropGrabbableObject(dynamic_cast<const DropGrabbableObject&>(args));
		break;
	case HeadsetInterfaceCommand::LISTEN_TRIGGER_STATE_CHANGE:
		HandleTriggerChange(dynamic_cast<const commands::TriggerStateChange&>(args));
		break;
	default:
		CollisionCollection::HandleCommand(args);
	}
}

void GrabbableObjectHandler::HandleDropGrabbableObject(const DropGrabbableObject& args) {
}

void GrabbableObjectHandler::HandleTriggerChange(const commands::TriggerStateChange& args) {
	if (args.is_pulled_) {
		ActorId collided_actor = FindCollidingActor(
			CollisionShape(args.controller_position_, 0));
		if (collided_actor != ActorId::UnsetId) {
			// Alert to the actor that it was grabbed.
			GetScene().MakeCommandAfter(
				GetScene().FrontOfCommands(),
				Command(
					Target(collided_actor),
					make_unique<ObjectGrabbed>(true, args.controller_number_, args.controller_position_)));
			SetGrabbedActor(args.controller_number_, collided_actor);
		}
	} else {
		if (grabbed_actor_[args.controller_number_] != ActorId::UnsetId) {
			DropController(args.controller_number_);
		}
	}
}

void GrabbableObjectHandler::DropActor(ActorId actor_id) {
	if (actor_id == ActorId::UnsetId) {
		return;
	}
	for (unsigned char controller_number = 0; controller_number < grabbed_actor_.size(); controller_number++) {
		if (grabbed_actor_[controller_number] == actor_id) {
			DropController(controller_number);
		}
	}
}

void GrabbableObjectHandler::DropController(unsigned char controller_number) {
	// Alert to the actor that it was released.
	GetScene().MakeCommandAfter(
		GetScene().FrontOfCommands(),
		Command(
			Target(grabbed_actor_[controller_number]),
			make_unique<ObjectGrabbed>(false, controller_number)));
	UnsetGrabbedActor(controller_number);
}

void GrabbableObjectHandler::SetGrabbedActor(unsigned char controller_number, ActorId grabbed_actor) {
	grabbed_actor_[controller_number] = grabbed_actor;
	GetScene().MakeCommandAfter(
		GetScene().FrontOfCommands(),
		Command(
			Target(GetScene().FindByName("HeadsetInterface")),
			make_unique<commands::HeadsetListenerRegistration>(
				true, grabbed_actor_[controller_number], actors::HeadsetInterface::ListenerId::CONTROLLER_MOVEMENT, controller_number)));
}

void GrabbableObjectHandler::UnsetGrabbedActor(unsigned char controller_number) {
	GetScene().MakeCommandAfter(
		GetScene().FrontOfCommands(),
		Command(
			Target(GetScene().FindByName("HeadsetInterface")),
			make_unique<commands::HeadsetListenerRegistration>(
				false, grabbed_actor_[controller_number], actors::HeadsetInterface::ListenerId::CONTROLLER_MOVEMENT, controller_number)));
	grabbed_actor_[controller_number] = ActorId::UnsetId;
}

}