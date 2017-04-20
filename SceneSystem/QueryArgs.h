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

	IdType Type() const {return id_;}
	IdType id_;
	string Name() const { return QueryRegistry::GetRegistry().LookupNameConst(id_); }
};

class QueryArgs
{
public:
	QueryArgs() : type_(0) {}
	QueryArgs(QueryType type);
	virtual ~QueryArgs() {}

	QueryArgs(const QueryArgs&) = delete;
	QueryArgs operator=(const QueryArgs&) = delete;

	IdType Type() const {return type_.Type();}

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