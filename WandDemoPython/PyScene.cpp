#include "stdafx.h"
#include "PyScene.h"
#include "PyCommandAndQuery.h"

namespace PyScene {
game_scene::ActorId AddActor(game_scene::Scene& self, object new_actor) {
	return self.AddActor(extract<game_scene::ActorImpl&>(new_actor));
}

game_scene::ActorId AddActorAfter(game_scene::Scene& self, object new_actor, game_scene::CommandQueueLocation initialize_after) {
	return self.AddActor(extract<game_scene::ActorImpl&>(new_actor), initialize_after);
}

boost::python::tuple AddActorReturnInitialize(game_scene::Scene& self, object new_actor) {
	tuple<game_scene::ActorId, game_scene::CommandQueueLocation> result = self.AddActorReturnInitialize(extract<game_scene::ActorImpl&>(new_actor));
	return boost::python::make_tuple(std::get<0>(result), std::get<1>(result));
}

boost::python::tuple AddActorAfterReturnInitialize(game_scene::Scene& self, object new_actor, game_scene::CommandQueueLocation initialize_after) {
	tuple<game_scene::ActorId, game_scene::CommandQueueLocation> result = self.AddActorReturnInitialize(extract<game_scene::ActorImpl&>(new_actor), initialize_after);
	return boost::python::make_tuple(std::get<0>(result), std::get<1>(result));
}


object PythonObjectifyQueryResult(unique_ptr<game_scene::QueryResult> raw_result) {
	PyQueryResult* cast_result = dynamic_cast<PyQueryResult*>(raw_result.get());
	if (cast_result) {
		return cast_result->self_;
	}
	return object(std::auto_ptr<game_scene::QueryResult>(raw_result.release()));
}

object AskQuery(game_scene::Scene& self, const game_scene::Target& target, const game_scene::QueryArgs& args) {
	return PythonObjectifyQueryResult(self.AskQuery(target, args));
}

object AskQueryWithPythonArgs(game_scene::Scene& self, const game_scene::Target& target, object args) {
	return PythonObjectifyQueryResult(self.AskQuery(target, PyQueryArgs(args)));
}

}  // PyScene
