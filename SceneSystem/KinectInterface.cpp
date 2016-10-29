#pragma once

#include "stdafx.h"

#include "VRBackend/Headset.h"

#include "KinectInterface.h"
#include "InputCommandArgs.h"
#include "Scene.h"
#include "Registry.h"

namespace game_scene {
REGISTER_COMMAND(KinectInterfaceCommand, REGISTER_LISTEN_FOR_BODIES);
REGISTER_COMMAND(KinectInterfaceCommand, BODIES_DISCOVERED);

REGISTER_QUERY(KinectInterfaceQuery, GET_BODY_LOOKUP);


BodyLookup::BodyLookup(Headset& headset) : headset_(headset) {}

Body& BodyLookup::GetBody(TrackingId id) {
	return headset_.GetBody(id);
}

namespace actors {

void KinectInterface::AddedToScene() {
	new_bodies_listener_group_ = scene_->AddActorGroup();
}

void KinectInterface::HandleCommand(const CommandArgs& args) {
	switch (args.Type()) {
	case KinectInterfaceCommand::REGISTER_LISTEN_FOR_BODIES:
	{
		const commands::ListenForBodies& registration = dynamic_cast<const commands::ListenForBodies&>(args);
		if (registration.register_not_unregister_) {
			scene_->AddActorToGroup(registration.actor_to_register_, new_bodies_listener_group_);
		} else {
			scene_->RemoveActorFromGroup(registration.actor_to_register_, new_bodies_listener_group_);
		} 
	}
	break;
	case InputCommand::TICK:
	{
		vector<TrackingId> new_bodies = headset_.GetNewTrackedIds();
		if (!new_bodies.empty()) {
			scene_->MakeCommandAfter(scene_->FrontOfCommands(), Command(
				Target(new_bodies_listener_group_),
				make_unique<commands::BodiesDiscovered>(
					std::move(new_bodies))));
		}
	}
	break;
	}
}

unique_ptr<QueryResult> KinectInterface::AnswerQuery(const QueryArgs& args) {
	switch (args.Type()) {
	case KinectInterfaceQuery::GET_BODY_LOOKUP:
	{
		return make_unique<QueryResultWrapped<BodyLookup>>(KinectInterfaceQuery::GET_BODY_LOOKUP, BodyLookup(headset_));
	}
	break;
	default:
		return nullptr;
	}
}

BodyLookup KinectInterface::GetBodyLookup(Scene* scene) {
	return dynamic_cast<QueryResultWrapped<BodyLookup>*>(scene->AskQuery(Target(scene->FindByName("KinectInterface")), QueryArgs(KinectInterfaceQuery::GET_BODY_LOOKUP)).get())->data_;
}

}  // actors
}  // game_scene
