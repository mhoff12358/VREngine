#pragma once

#include "stdafx.h"

namespace game_scene {

enum class QueryType {
	
};

class QueryArgs
{
public:
	QueryArgs(QueryType type);
	virtual ~QueryArgs() = 0;

	QueryType type_;
};
}  // game_scene