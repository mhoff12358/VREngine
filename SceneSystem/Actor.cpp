#include "stdafx.h"
#include "Actor.h"
#include "Scene.h"
#include "QueryArgs.h"
#include "CommandArgs.h"

namespace game_scene {

Actor::Actor() : id_(ActorId::UnsetId), scene_(nullptr)
{
}

Actor::~Actor() {
}

void Actor::HandleCommand(const CommandArgs& args) {
}

unique_ptr<QueryResult> Actor::AnswerQuery(const QueryArgs& args) {
	return make_unique<QueryResult>(QueryType::EMPTY);
}

void Actor::AddedToScene() {
}

void Actor::PrepareToDie() {
}

void Actor::DependencyDying(const ActorId& dying_id) {
}

void Actor::RegisterDependency(const Target& target) {
	scene_->RegisterDependency(Target(id_), target);
}

unique_ptr<QueryResult> Actor::AskQuery(const Target& target, const QueryArgs& args) {
	return scene_->AskQuery(target, args);
}

void Actor::FailToHandleCommand(const CommandArgs& args) {
	// The ADDED_TO_SCENE command is expected to be allowed to be unhandled.
	if (args.Type() != CommandType::ADDED_TO_SCENE) {
		std::cout <<
			"ACTOR ID: " << id_.id_ <<
			" HAS FAILED TO HANDLE COMMAND: " << args.Type() << std::endl;
	}
}

void Actor::SetId(ActorId id) {
	id_ = id;
}

void Actor::SetScene(Scene* scene) {
	scene_ = scene;
}

Scene& Actor::GetScene() {
	return *scene_;
}

}  // game_scene