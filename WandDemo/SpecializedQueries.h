#pragma once

#include "QueryResult.h"
#include "QueryArgs.h"

namespace game_scene {
namespace queries {

class GraphicsResourceResultType : public ResultType {
public:
	enum GraphicsResourceResultTypeId : int {
		GRAPHICS_RESOURCE_REQUEST = GRAPHICS_RESOURCES,
	};
};

class GraphicsResourceQueryType : public QueryType {
public:
	enum GraphicsResourceQueryTypeId : int {
		GRAPHICS_RESOURCE_REQUEST = GRAPHICS_RESOURCES,
	};
};

}  // queries
}  // game_scene