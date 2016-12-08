#include "stdafx.h"
#include "PyScene.h"
#include "PyQueryResult.h"

namespace PyScene {

ActorCreationBundle<game_scene::Actor> AddAndConstructActorDum(game_scene::Scene& self) {
	auto actor = make_unique<game_scene::Actor>();
	auto* actor_pointer = actor.get();
	auto result = self.AddActorReturnInitialize(std::move(actor));
	return ActorCreationBundle<game_scene::Actor>(actor_pointer, get<0>(result), get<1>(result));
}

game_scene::ActorId AddActor(game_scene::Scene& self, PyActor* new_actor) {
	return self.AddActor(unique_ptr<game_scene::Actor>(new_actor));
}

game_scene::ActorId AddActorAfter(game_scene::Scene& self, PyActor* new_actor, game_scene::CommandQueueLocation initialize_after) {
	return self.AddActor(unique_ptr<game_scene::Actor>(new_actor), initialize_after);
}

boost::python::tuple AddActorReturnInitialize(game_scene::Scene& self, PyActor* new_actor) {
	tuple<game_scene::ActorId, game_scene::CommandQueueLocation> result = self.AddActorReturnInitialize(unique_ptr<game_scene::Actor>(new_actor));
	return boost::python::make_tuple(std::get<0>(result), std::get<1>(result));
}

boost::python::tuple AddActorAfterReturnInitialize(game_scene::Scene& self, PyActor* new_actor, game_scene::CommandQueueLocation initialize_after) {
	tuple<game_scene::ActorId, game_scene::CommandQueueLocation> result = self.AddActorReturnInitialize(unique_ptr<game_scene::Actor>(new_actor), initialize_after);
	return boost::python::make_tuple(std::get<0>(result), std::get<1>(result));
}

//std::auto_ptr<game_scene::QueryResult> AskQuery(game_scene::Scene& self, const game_scene::Target& target, const game_scene::QueryArgs& args) {
//	return std::auto_ptr<game_scene::QueryResult>(self.AskQuery(target, args).release());
//}

object AskQuery(game_scene::Scene& self, const game_scene::Target& target, const game_scene::QueryArgs& args) {
	game_scene::QueryResult* a = self.AskQuery(target, args).release();
	PyQueryResult* b = dynamic_cast<PyQueryResult*>(a);
	if (b) {
		return b->self_;
	}
	object c;
	c = object(std::auto_ptr<game_scene::QueryResult>(a));
	return c;
}

}  // PyScene
