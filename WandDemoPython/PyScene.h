#pragma once
#include "stdafx.h"

#include "SceneSystem/Scene.h"
#include "PyActor.h"

namespace PyScene {
game_scene::ActorId AddActor(game_scene::Scene& self, PyActor* new_actor) {
	return self.AddActor(unique_ptr<game_scene::Shmactor>(new_actor));
}

game_scene::ActorId AddActorAfter(game_scene::Scene& self, PyActor* new_actor, game_scene::CommandQueueLocation initialize_after) {
	return self.AddActor(unique_ptr<game_scene::Shmactor>(new_actor), initialize_after);
}

tuple<game_scene::ActorId, game_scene::CommandQueueLocation> AddActorReturnInitialize(game_scene::Scene& self, PyActor* new_actor) {
	return self.AddActorReturnInitialize(unique_ptr<game_scene::Shmactor>(new_actor));
}

tuple<game_scene::ActorId, game_scene::CommandQueueLocation> AddActorAfterReturnInitialize(game_scene::Scene& self, PyActor* new_actor, game_scene::CommandQueueLocation initialize_after) {
	return self.AddActorReturnInitialize(unique_ptr<game_scene::Shmactor>(new_actor), initialize_after);
}

template <typename T>
game_scene::CommandQueueLocation MakeCommandAfter(game_scene::Scene& self, game_scene::CommandQueueLocation location, game_scene::Target target, std::auto_ptr<T> args) {
	return self.MakeCommandAfter(location, game_scene::Command(target, unique_ptr<game_scene::CommandArgs>(dynamic_cast<game_scene::CommandArgs*>(args.release()))));
}

std::auto_ptr<game_scene::QueryResult> AskQuery(game_scene::Scene& self, const game_scene::Target& target, const game_scene::QueryArgs& args) {
	return std::auto_ptr<game_scene::QueryResult>(self.AskQuery(target, args).release());
}

}  // PyScene