#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "SpecializedActors.h"
#include "QueryArgs.h"
#include "QueryResult.h"
#include "CommandArgs.h"
#include "BoostPythonWrapper.h"
#include "Pose.h"
#include "Registry.h"

namespace game_scene {

class NichijouCommand {
public:
	DECLARE_COMMAND(NichijouCommand, TELL_VERTEX_ABOUT_EDGE);
	DECLARE_COMMAND(NichijouCommand, SET_VERTEX_LOCATION);
	DECLARE_COMMAND(NichijouCommand, MOVE_VERTEX_LOCATION);
	DECLARE_COMMAND(NichijouCommand, SET_EDGE_LOCATION);
	DECLARE_COMMAND(NichijouCommand, SET_VISIBLE);
	DECLARE_COMMAND(NichijouCommand, TELL_TIMELINE_POSITION);
};

namespace commands {

class TimelinePosition : public CommandArgs {
public:
	TimelinePosition(float position, float max_position) :
		CommandArgs(NichijouCommand::TELL_TIMELINE_POSITION),
		position_(position), max_position_(max_position) {}

	float position_;
	float max_position_;
};

}  // commands

class NichijouQuery {
public:
	DECLARE_QUERY(NichijouQuery, GET_VERTEX);
};

namespace actors {

class NichijouGraph : public Shmactor {
public:
	void HandleCommand(const CommandArgs& args) override;
	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) override;

private:
	void CreateGraphicsResources();
	void MoveTimelinePosition(float timeline_delta);

	ActorId headset_interface_;
	object configuration_;
	object graph_;
	map<string, ActorId> vertex_actors_;
	vector<ActorId> edge_actors_;
	string edge_model_name_;
	ActorId timeline_graphics_;
	// These colors are vectors so that they can be used as input to the shader settings.
	vector<float> timeline_color_1_;
	vector<float> timeline_color_2_;

	float timeline_position_ = 0.0f;
	float timeline_max_position_ = 1.0f;
};

class NichijouVertex : public Shmactor {
public:
	NichijouVertex(object configuration, object vertex, ActorId graph) : configuration_(configuration), vertex_(vertex), graph_(graph) {}

	void HandleCommand(const CommandArgs& args) override;

private:
	void UpdateAllEdgeLocations();
	void UpdateEdgeLocation(ActorId edge_actor);

	object configuration_;
	object vertex_;
	ActorId graph_;
	vector<ActorId> edges_;
	ActorId vertex_graphics_;
	Location location_;
	bool visible_ = true;
};

class NichijouEdge : public Shmactor {
public:
	NichijouEdge(object configuration, object edge, ActorId graph, string model_name) :
		configuration_(configuration), edge_(edge), graph_(graph), model_name_(model_name) {}

	void HandleCommand(const CommandArgs& args) override;

private:
	object configuration_;
	object edge_;
	ActorId graph_;
	ActorId u_actor_;
	ActorId v_actor_;
	Location u_location_;
	Location v_location_;
	string model_name_;
	ActorId edge_graphics_;
	bool visible_ = true;
};

}  // actors
}  // game_scene