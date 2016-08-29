#include "stdafx.h"
#include "QueryResult.h"

namespace game_scene {

QueryResult::QueryResult(QueryType type) : type_(type) {
}

MultipleQueryResult::MultipleQueryResult() : QueryResult(QueryType::MULTIPLE) {
}

void MultipleQueryResult::AddResult(ActorId actor_id, unique_ptr<QueryResult> result) {
	results_map_.insert(pair<ActorId, unique_ptr<QueryResult>>(actor_id, move(result)));
}

}  // game_scene