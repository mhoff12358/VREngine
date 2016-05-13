#pragma once

#include "stdafx.h"

#include "ActorId.h"

namespace game_scene {

enum class ResultType {
	EMPTY = 0,
	MULTIPLE = 1,
};

class QueryResult
{
public:
	QueryResult(ResultType type);
	virtual ~QueryResult() {};

	static QueryResult EmptyResult();

	ResultType type_;
};

class MultipleQueryResult : public QueryResult {
public:
	MultipleQueryResult();
	void AddResult(ActorId actor_id, unique_ptr<QueryResult> result);

private:
	map<ActorId, unique_ptr<QueryResult>> results_map_;
};
}  // game_scene