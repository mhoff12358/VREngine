#pragma once
#include "stdafx.h"

#include "SceneSystem/Scene.h"
#include "PyActor.h"

namespace PyScene {

/*template <typename ActorSubclass>
game_scene::ActorId AddAndConstructActor(game_scene::Scene& self) {
	return self.AddActor(make_unique<ActorSubclass>());
}*/

/*template <typename ActorSubclass>
game_scene::ActorId AddAndConstructActorAfter(game_scene::Scene& self, game_scene::CommandQueueLocation initialize_after) {
	return self.AddActor(make_unique<ActorSubclass>(), initialize_after);
}*/

template <typename ActorType>
class ActorCreationBundle {
public:
	ActorCreationBundle(ActorType* actor, game_scene::ActorId id, game_scene::CommandQueueLocation initialized_at)
		: actor_(actor), id_(id), initialized_at_(initialized_at) {}

	ActorType* actor_;
	game_scene::ActorId id_;
	game_scene::CommandQueueLocation initialized_at_;
};

ActorCreationBundle<game_scene::Shmactor> AddAndConstructActorDum(game_scene::Scene& self) {
	auto actor = make_unique<game_scene::Shmactor>();
	auto* actor_pointer = actor.get();
	auto result = self.AddActorReturnInitialize(std::move(actor));
	return ActorCreationBundle<game_scene::Shmactor>(actor_pointer, get<0>(result), get<1>(result));
}

template <typename ActorSubclass>
ActorCreationBundle<ActorSubclass> AddAndConstructActor(game_scene::Scene& self) {
	auto actor = make_unique<ActorSubclass>();
	auto* actor_pointer = actor.get();
	auto result = self.AddActorReturnInitialize(std::move(actor));
	return ActorCreationBundle<ActorSubclass>(actor_pointer, get<0>(result), get<1>(result));
}

template <typename ActorSubclass>
ActorCreationBundle<ActorSubclass> AddAndConstructActorAfter(game_scene::Scene& self, game_scene::CommandQueueLocation initialize_after) {
	auto actor = make_unique<ActorSubclass>();
	auto* actor_pointer = actor.get();
	auto result = self.AddActorReturnInitialize(std::move(actor), initialize_after);
	return ActorCreationBundle<ActorSubclass>(actor_pointer, get<0>(result), get<1>(result));
}

template <typename ActorSubclass>
void AddActorSubclassCreation(class_<game_scene::Scene, boost::noncopyable>& scene_class, const string& subclass_name) {
	class_<ActorCreationBundle<ActorSubclass>>(("ActorSubclass" + subclass_name).c_str(), no_init)
		.def_readonly("actor", &ActorCreationBundle<ActorSubclass>::actor_)
		.def_readonly("id", &ActorCreationBundle<ActorSubclass>::id_)
		.def_readonly("initialzed_at", &ActorCreationBundle<ActorSubclass>::initialized_at_);
	scene_class
		.def(("AddAndConstruct" + subclass_name).c_str(), &AddAndConstructActor<ActorSubclass>)
		.def(("AddAndConstruct" + subclass_name + "After").c_str(), &AddAndConstructActorAfter<ActorSubclass>);
}

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