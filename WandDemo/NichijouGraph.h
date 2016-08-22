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
};

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

	ActorId headset_interface_;
	object configuration_;
	object graph_;
	map<string, ActorId> vertex_actors_;
	vector<ActorId> edge_actors_;
	string edge_model_name_;
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
};

}  // actors
}  // game_scene