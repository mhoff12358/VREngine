#include "stdafx.h"

#include "PyQueryResult.h"

std::auto_ptr<game_scene::QueryResult> ExtractRawQueryResult(std::auto_ptr<game_scene::QueryResult> in) {
	return in;
}

std::auto_ptr<game_scene::QueryResult> ExtractQueryResult(object in) {
	std::auto_ptr<PyQueryResult> a = extract<std::auto_ptr<PyQueryResult>>(in);
	auto* ptr = dynamic_cast<game_scene::QueryResult*>(a.release());
	return std::auto_ptr<game_scene::QueryResult>(ptr);
}
