#include "stdafx.h"
#include "Shmactor.h"
#include "Scene.h"

namespace game_scene {
Scene* Shmactor::scene_ = nullptr;

Shmactor::Shmactor() : id_(ActorId::UnsetId)
{
}

Shmactor::~Shmactor() {
}

void Shmactor::HandleCommand(const CommandArgs& args) {
}

unique_ptr<QueryResult> Shmactor::AnswerQuery(const QueryArgs& args) {
	return make_unique<QueryResult>(ResultType::EMPTY);
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

CommandQueueLocation Shmactor::MakeCommandAfter(CommandQueueLocation location, const Target& target, unique_ptr<CommandArgs> args) {
	return MakeCommandAfter(location, target, move(args));
}

unique_ptr<QueryResult> Shmactor::AskQuery(const Target& target, unique_ptr<QueryArgs> args) {
	return scene_->AskQuery(target, move(args));
}

void Shmactor::FailToHandleCommand(const CommandArgs& args) {
	std::cout <<
		"ACTOR ID: " << id_.id_ <<
		" HAS FAILED TO HANDLE COMMAND: " << args.Type() << std::endl;
}

void Shmactor::SetId(ActorId id) {
	id_ = id;
}

void Shmactor::SetScene(Scene* scene) {
	scene_ = scene;
}

}  // game_scene