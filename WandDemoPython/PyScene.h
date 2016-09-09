#pragma once
#include "stdafx.h"

#include "SceneSystem/Scene.h"
#include "PyActor.h"

namespace PyScene {
game_scene::ActorId AddActor(game_scene::Scene& self, std::auto_ptr<PyActor> new_actor) {
	return self.AddActor(unique_ptr<game_scene::Shmactor>(new_actor.release()));
}

game_scene::ActorId AddActorAfter(game_scene::Scene& self, std::auto_ptr<PyActor> new_actor, game_scene::CommandQueueLocation initialize_after) {
	return self.AddActor(unique_ptr<game_scene::Shmactor>(new_actor.release()), initialize_after);
}

tuple<game_scene::ActorId, game_scene::CommandQueueLocation> AddActorReturnInitialize(game_scene::Scene& self, std::auto_ptr<PyActor> new_actor) {
	return self.AddActorReturnInitialize(unique_ptr<game_scene::Shmactor>(new_actor.release()));
}

tuple<game_scene::ActorId, game_scene::CommandQueueLocation> AddActorAfterReturnInitialize(game_scene::Scene& self, std::auto_ptr<PyActor> new_actor, game_scene::CommandQueueLocation initialize_after) {
	return self.AddActorReturnInitialize(unique_ptr<game_scene::Shmactor>(new_actor.release()), initialize_after);
}

game_scene::CommandQueueLocation MakeCommandAfter(game_scene::Scene& self, game_scene::CommandQueueLocation location, game_scene::Command& command) {
	return self.MakeCommandAfter(location, move(command));
}

int Test1(game_scene::Scene& self) {
	return 7;
}

int Test2(std::auto_ptr<PyActor> act) {
	return reinterpret_cast<int>(act.get());
}
}  // PyScene