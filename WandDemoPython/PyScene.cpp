#include "stdafx.h"
#include "PyScene.h"

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

tuple<game_scene::ActorId, game_scene::CommandQueueLocation> AddActorReturnInitialize(game_scene::Scene& self, PyActor* new_actor) {
	return self.AddActorReturnInitialize(unique_ptr<game_scene::Actor>(new_actor));
}

tuple<game_scene::ActorId, game_scene::CommandQueueLocation> AddActorAfterReturnInitialize(game_scene::Scene& self, PyActor* new_actor, game_scene::CommandQueueLocation initialize_after) {
	return self.AddActorReturnInitialize(unique_ptr<game_scene::Actor>(new_actor), initialize_after);
}

std::auto_ptr<game_scene::QueryResult> AskQuery(game_scene::Scene& self, const game_scene::Target& target, const game_scene::QueryArgs& args) {
	return std::auto_ptr<game_scene::QueryResult>(self.AskQuery(target, args).release());
}

}  // PyScene
