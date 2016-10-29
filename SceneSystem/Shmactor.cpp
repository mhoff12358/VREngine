#include "stdafx.h"
#include "Shmactor.h"
#include "Scene.h"
#include "QueryArgs.h"
#include "CommandArgs.h"

namespace game_scene {

Shmactor::Shmactor() : id_(ActorId::UnsetId), scene_(nullptr)
{
}

Shmactor::~Shmactor() {
}

void Shmactor::HandleCommand(const CommandArgs& args) {
}

unique_ptr<QueryResult> Shmactor::AnswerQuery(const QueryArgs& args) {
	return make_unique<QueryResult>(QueryType::EMPTY);
}

void Shmactor::AddedToScene() {
}

void Shmactor::PrepareToDie() {
}

void Shmactor::DependencyDying(const ActorId& dying_id) {
}

void Shmactor::RegisterDependency(const Target& target) {
	scene_->RegisterDependency(Target(id_), target);
}

unique_ptr<QueryResult> Shmactor::AskQuery(const Target& target, const QueryArgs& args) {
	return scene_->AskQuery(target, args);
}

void Shmactor::FailToHandleCommand(const CommandArgs& args) {
	// The ADDED_TO_SCENE command is expected to be allowed to be unhandled.
	if (args.Type() != CommandType::ADDED_TO_SCENE) {
		std::cout <<
			"ACTOR ID: " << id_.id_ <<
			" HAS FAILED TO HANDLE COMMAND: " << args.Type() << std::endl;
	}
}

void Shmactor::SetId(ActorId id) {
	id_ = id;
}

void Shmactor::SetScene(Scene* scene) {
	scene_ = scene;
}

Scene& Shmactor::GetScene() {
	return *scene_;
}

}  // game_scene