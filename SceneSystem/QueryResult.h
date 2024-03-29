#pragma once

#include "stdafx.h"

#include "ActorId.h"
#include "QueryArgs.h"

namespace game_scene {

class QueryResult
{
public:
	explicit QueryResult(QueryType type);
	virtual ~QueryResult() {}

	QueryResult(const QueryResult&) = delete;
	QueryResult operator=(const QueryResult&) = delete;

	int64_t Type() const {return type_.Type();}
	string Name() const { return type_.Name(); }

	QueryType type_;
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
	QueryResultWrapped(QueryType type, WrappedType data)
		: QueryResult(type), data_(data) {}
	virtual ~QueryResultWrapped() {}

	WrappedType& GetData() { return data_; }

	WrappedType data_;
};

class EmptyQueryResult : public QueryResult {
public:
	EmptyQueryResult() : QueryResult(QueryType::EMPTY) {}
};

}  // game_scene