#pragma once

#include "stdafx.h"

namespace game_scene {

class QueryType {
public:
	QueryType(int id) : id_(id) {}

	enum QueryTypeId : int {
		GRAPHICS_RESOURCES = 100,
		LUA_RUNTIME = 200,
	};

	int Type() {return id_;}
	int id_;
};

class QueryArgs
{
public:
	QueryArgs(QueryType type);
	virtual ~QueryArgs() {}

	int Type() const {return type_.id_;}

private:
	QueryType type_;
};

template <typename WrappedType>
class QueryArgsWrapped : public QueryArgs {
public:
	QueryArgsWrapped(QueryType type, WrappedType data)
		: QueryArgs(type), data_(data) {}
	virtual ~QueryArgsWrapped() {}

	WrappedType data_;
};

class EmptyQuery : public QueryArgs {
public:
	EmptyQuery(QueryType type) : QueryArgs(type) {}
	virtual ~EmptyQuery() {}
};

}  // game_scene