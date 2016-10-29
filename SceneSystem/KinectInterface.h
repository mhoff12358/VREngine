#pragma once

#include "stdafx.h"

#include "Shmactor.h"
#include "Kinect.h"
#include "CommandArgs.h"
#include "VRBackend/Pose.h"
#include "ActorId.h"

class Headset;
class Body;
typedef uint64_t TrackingId;

namespace game_scene {

class BodyLookup {
public:
	BodyLookup(Headset& headset);

	Body& GetBody(TrackingId id);
private:
	Headset& headset_;
};

namespace actors {

class KinectInterface : public Shmactor {
public:
	KinectInterface(Headset& headset) :
		headset_(headset)
	{}

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) override;
	void HandleCommand(const CommandArgs& args) override;
	void AddedToScene() override;

	static BodyLookup GetBodyLookup(Scene* scene);

private:

	ActorId new_bodies_listener_group_;
	Headset& headset_;
};

}  // actors

class KinectInterfaceCommand {
public:
	DECLARE_COMMAND(HeadsetInterfaceCommand, REGISTER_LISTEN_FOR_BODIES);
	DECLARE_COMMAND(HeadsetInterfaceCommand, BODIES_DISCOVERED);
};

class KinectInterfaceQuery {
public:
	DECLARE_QUERY(KinectInterfaceQuery, GET_BODY_LOOKUP)
};

namespace queries {

}  // queries

namespace commands {

class ListenForBodies : public CommandArgs {
public:
	ListenForBodies(ActorId actor_to_register, bool register_not_unregister) :
		CommandArgs(KinectInterfaceCommand::REGISTER_LISTEN_FOR_BODIES),
		actor_to_register_(actor_to_register),
		register_not_unregister_(register_not_unregister) {}

	ActorId actor_to_register_;
	bool register_not_unregister_;
};

class BodiesDiscovered : public CommandArgs {
public:
	BodiesDiscovered(vector<TrackingId>&& tracking_ids) :
		CommandArgs(KinectInterfaceCommand::BODIES_DISCOVERED),
		tracking_ids_(tracking_ids) {}

	vector<TrackingId> tracking_ids_;
};

}  // commands
}  // game_scene
