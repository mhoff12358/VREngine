#include "PyActor.h"

unique_ptr<game_scene::QueryResult> PyActor::AnswerQuery(const game_scene::QueryArgs& args) {
	//std::auto_ptr<game_scene::QueryResult> a = extract<std::auto_ptr<game_scene::QueryResult>>(PyAnswerQuery(args));
	return unique_ptr<game_scene::QueryResult>(PyAnswerQuery(args).release());
}