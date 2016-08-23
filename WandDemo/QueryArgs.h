#pragma once

#include "stdafx.h"
#include "Registry.h"

namespace game_scene {

class QueryType {
public:
	QueryType(IdType id) : id_(id) {}
	QueryType(QueryRegistry id) : id_(id.value_) {}

	DECLARE_QUERY(QueryType, EMPTY);
	DECLARE_QUERY(QueryType, MULTIPLE);

	IdType Type() {return id_;}
	IdType id_;
};

class QueryArgs
{
public:
	QueryArgs(QueryType type);
	virtual ~QueryArgs() {}

	IdType Type() const {return type_.id_;}

private:
	QueryType type_;
};

template <typename WrappedType>
class WrappedQueryArgs : public QueryArgs {
public:
	WrappedQueryArgs(QueryType type, WrappedType data)
		: QueryArgs(type), data_(data) {}
	virtual ~WrappedQueryArgs() {}

	WrappedType data_;
};

class EmptyQuery : public QueryArgs {
public:
	EmptyQuery(QueryType type) : QueryArgs(type) {}
	virtual ~EmptyQuery() {}
};

}  // game_scene