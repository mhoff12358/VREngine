#include "stdafx.h"
#include "QueryArgs.h"
#include "Registry.h"

namespace game_scene {

REGISTER_QUERY(QueryType, EMPTY);
REGISTER_QUERY(QueryType, MULTIPLE);

QueryArgs::QueryArgs(QueryType type) : type_(type) {
}
}  // game_scene