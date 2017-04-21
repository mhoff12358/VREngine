#include "stdafx.h"
#include "QueryArgs.h"
#include "Registry.h"

FORCE_LINK_THIS(QueryArgs)

namespace game_scene {

REGISTER_QUERY(QueryType, EMPTY);
REGISTER_QUERY(QueryType, MULTIPLE);

QueryArgs::QueryArgs(QueryType type) : type_(type) {
}
}  // game_scene