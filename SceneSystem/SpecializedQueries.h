#pragma once

#include "QueryResult.h"
#include "QueryArgs.h"

namespace game_scene {

class GraphicsResourceQuery {
public:
	DECLARE_QUERY(GraphicsResourceQuery, GRAPHICS_RESOURCE_REQUEST);
};

}  // game_scene