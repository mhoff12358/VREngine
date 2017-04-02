#include "stdafx.h"

#include "PyActor.h"

unique_ptr<game_scene::QueryResult> PyActorImpl::AnswerQuery(const game_scene::QueryArgs& args) {
	return unique_ptr<game_scene::QueryResult>(PyAnswerQuery(args).release());
}