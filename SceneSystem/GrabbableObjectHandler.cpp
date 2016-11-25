#include "stdafx.h"
#include "GrabbableObjectHandler.h"

#include "HeadsetInterface.h"
#include "Scene.h"

namespace game_scene {
GrabbableObjectHandler::GrabbableObjectHandler() {

}

void GrabbableObjectHandler::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case CommandType::ADDED_TO_SCENE:
		scene_->MakeCommandAfter(
			scene_->FrontOfCommands(),
			Command(
				Target(scene_->FindByName("HeadsetInterface")),
				make_unique<commands::HeadsetListenerRegistration>(
					true, id_, actors::HeadsetInterface::ListenerId::TRIGGER_STATE_CHANGE)));
		break;
	case GrabbableObjectCommand::ADD_GRABBABLE_OBJECT:
		HandleAddGrabbableObject(dynamic_cast<const AddGrabbableObject&>(args));
		break;
	case GrabbableObjectCommand::REMOVE_GRABBABLE_OBJECT:
		HandleRemoveGrabbableObject(dynamic_cast<const RemoveGrabbableObject&>(args));
		break;
	case GrabbableObjectCommand::ENDISABLE_GRABBABLE_OBJECT:
		HandleEnDisableGrabbableObject(dynamic_cast<const EnDisableGrabbableObject&>(args));
		break;
	case GrabbableObjectCommand::REPOSE_GRABBABLE_OBJECT:
		HandleReposeGrabbableObject(dynamic_cast<const ReposeGrabbableObject&>(args));
		break;
	case HeadsetInterfaceCommand::LISTEN_TRIGGER_STATE_CHANGE:
		HandleTriggerChange(dynamic_cast<const commands::TriggerStateChange&>(args));
		break;
	}
}

void GrabbableObjectHandler::HandleAddGrabbableObject(const AddGrabbableObject& args) {
	// If this actor already has a held location then remove it first.
	if (grabbable_actors_.find(args.grabbable_actor_) != grabbable_actors_.end()) {
		RemoveGrabInformation(args.grabbable_actor_);
	}
	AddGrabInformation(args.grabbable_actor_, args.grab_shapes_);
}

void GrabbableObjectHandler::HandleReposeGrabbableObject(const ReposeGrabbableObject& args) {
	vector<CollisionShape>& collision_shapes = GetCollisionShapes(args.grabbable_actor_);
	if (args.shape_index_ == -1) {
		for (CollisionShape& shape : collision_shapes) {
			shape.SetPose(args.pose_);
		}
	} else {
		collision_shapes[args.shape_index_].SetPose(args.pose_);
	}
}

void GrabbableObjectHandler::HandleEnDisableGrabbableObject(const EnDisableGrabbableObject& args) {
	vector<CollisionShape>& collision_shapes = GetCollisionShapes(args.grabbable_actor_);
	if (args.shape_index_ == -1) {
		for (CollisionShape& shape : collision_shapes) {
			shape.EnDisable(args.enable_);
		}
	} else {
		collision_shapes[args.shape_index_].EnDisable(args.enable_);
	}
}

void GrabbableObjectHandler::HandleRemoveGrabbableObject(const RemoveGrabbableObject& args) {
	RemoveGrabInformation(args.grabbable_actor_);
}

void GrabbableObjectHandler::HandleTriggerChange(const commands::TriggerStateChange& args) {
	if (args.is_pulled_) {
		ActorId collided_actor = FindCollidingActor(
			CollisionShape(args.controller_position_, 0));
		if (collided_actor != ActorId::UnsetId) {
			// Alert to the actor that it was grabbed.
			scene_->MakeCommandAfter(
				scene_->FrontOfCommands(),
				Command(
					Target(collided_actor),
					make_unique<ObjectGrabbed>(true, args.controller_number_, args.controller_position_)));
			SetGrabbedActor(args.controller_number_, collided_actor);
		}
	} else {
		if (grabbed_actor_[args.controller_number_] != ActorId::UnsetId) {
			// Alert to the actor that it was released.
			scene_->MakeCommandAfter(
				scene_->FrontOfCommands(),
				Command(
					Target(grabbed_actor_[args.controller_number_]),
					make_unique<ObjectGrabbed>(false, args.controller_number_, args.controller_position_)));
			UnsetGrabbedActor(args.controller_number_);
		}
	}
}

void GrabbableObjectHandler::AddGrabInformation(ActorId actor_id, vector<CollisionShape> collision) {
	grab_collisions_.emplace_back(make_pair(actor_id, std::move(collision)));
	grabbable_actors_.insert(actor_id);
}

void GrabbableObjectHandler::RemoveGrabInformation(ActorId actor_id) {
	unsigned int remove_index = 0;
	while (
		(actor_id != grab_collisions_[remove_index].first) &&
		(remove_index != grab_collisions_.size())) {
		remove_index++;
	}
	if (remove_index == grab_collisions_.size()) {
		return;
	}
	// It's safe to grab the back without checking the size because if it were empty
	// then remove_index would have been always 0 and the previous check would have
	// returned
	grab_collisions_[remove_index] = std::move(grab_collisions_.back());
	grab_collisions_.pop_back();
	grabbable_actors_.erase(actor_id);
}

void GrabbableObjectHandler::SetGrabbedActor(unsigned char controller_number, ActorId grabbed_actor) {
	grabbed_actor_[controller_number] = grabbed_actor;
	scene_->MakeCommandAfter(
		scene_->FrontOfCommands(),
		Command(
			Target(scene_->FindByName("HeadsetInterface")),
			make_unique<commands::HeadsetListenerRegistration>(
				true, grabbed_actor_[controller_number], actors::HeadsetInterface::ListenerId::CONTROLLER_MOVEMENT, controller_number)));
}

void GrabbableObjectHandler::UnsetGrabbedActor(unsigned char controller_number) {
	scene_->MakeCommandAfter(
		scene_->FrontOfCommands(),
		Command(
			Target(scene_->FindByName("HeadsetInterface")),
			make_unique<commands::HeadsetListenerRegistration>(
				false, grabbed_actor_[controller_number], actors::HeadsetInterface::ListenerId::CONTROLLER_MOVEMENT, controller_number)));
	grabbed_actor_[controller_number] = ActorId::UnsetId;
}

ActorId GrabbableObjectHandler::FindCollidingActor(const CollisionShape& controller_shape) {
	for (const pair<ActorId, vector<CollisionShape>>& collidable_actor : grab_collisions_) {
		for (const CollisionShape& actor_shape : collidable_actor.second) {
			if (actor_shape.Intersect(controller_shape)) {
				return collidable_actor.first;
			}
		}
	}
	return ActorId::UnsetId;
}

vector<CollisionShape>& GrabbableObjectHandler::GetCollisionShapes(ActorId actor_id) {
	size_t index = 0;
	while (
		(actor_id != grab_collisions_[index].first) &&
		(index != grab_collisions_.size())) {
		index++;
	}
	if (index == grab_collisions_.size()) {
		grab_collisions_.emplace_back();
	}
	return grab_collisions_[index].second;
}

}