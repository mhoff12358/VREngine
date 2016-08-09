#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "SpecializedActors.h"
#include "QueryArgs.h"
#include "QueryResult.h"
#include "CommandArgs.h"
#include "BoostPythonWrapper.h"
#include "Pose.h"

namespace game_scene {

namespace queries {
class NichijouGraphQueryType : public QueryType {
public:
	enum NichijouGraphQueryTypeId : int {
		GET_VERTEX = NICHIJOU_GRAPH,
	};
};
}  // queries

namespace commands {
class NichijouGraphCommandType : public CommandType {
public:
	enum NichijouGraphCommandTypeId : int {
		TELL_VERTEX_ABOUT_EDGE = NICHIJOU_GRAPH,
		SET_VERTEX_LOCATION,
		MOVE_VERTEX_LOCATION,
		SET_EDGE_LOCATION,
	};
};
}  // commands

namespace actors {

class NichijouGraph : public Shmactor {
public:
	void HandleCommand(const CommandArgs& args) override;
	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) override;

	object graph_;
	map<string, ActorId> vertex_actors_;
	vector<ActorId> edge_actors_;
};

class NichijouVertex : public Shmactor {
public:
	NichijouVertex(object vertex, ActorId graph) : vertex_(vertex), graph_(graph) {}

	void HandleCommand(const CommandArgs& args) override;

	void UpdateAllEdgeLocations();
	void UpdateEdgeLocation(ActorId edge_actor);

	object vertex_;
	ActorId graph_;
	vector<ActorId> edges_;
	ActorId vertex_graphics_;
	Location location_;
};

class NichijouEdge : public Shmactor {
public:
	NichijouEdge(object edge, ActorId graph) : edge_(edge), graph_(graph) {}

	void HandleCommand(const CommandArgs& args) override;

	object edge_;
	ActorId graph_;
	ActorId u_actor_;
	ActorId v_actor_;
	Location u_location_;
	Location v_location_;
};

}  // actors
}  // game_scene