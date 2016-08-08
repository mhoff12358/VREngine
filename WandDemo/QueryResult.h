#pragma once

#include "stdafx.h"

#include "ActorId.h"

namespace game_scene {

class ResultType {
public:
	ResultType(int id) : id_(id) {}

	enum ResultTypeId : int {
		EMPTY = 0,
		MULTIPLE = 1,

		GRAPHICS_RESOURCES = 100,
		LUA_RUNTIME = 200,
	};

	int Type() {return id_;}
	int id_;
};

class QueryResult
{
public:
	explicit QueryResult(ResultType type);
	virtual ~QueryResult() {}

	static QueryResult EmptyResult();

	int Type() const {return type_.id_;}

private:
	ResultType type_;
};

class MultipleQueryResult : public QueryResult {
public:
	MultipleQueryResult();
	virtual ~MultipleQueryResult() {}
	void AddResult(ActorId actor_id, unique_ptr<QueryResult> result);

private:
	map<ActorId, unique_ptr<QueryResult>> results_map_;
};

template <typename WrappedType>
class QueryResultWrapped : public QueryResult {
public:
	QueryResultWrapped(ResultType type, WrappedType data)
		: QueryResult(type), data_(data) {}
	virtual ~QueryResultWrapped() {}

	WrappedType data_;
};

}  // game_scene