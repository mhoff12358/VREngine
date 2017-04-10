#include "stdafx.h"
#include "Actor.h"
#include "Scene.h"
#include "QueryArgs.h"
#include "CommandArgs.h"

namespace game_scene {

IActor::IActor() : id_(ActorId::UnsetId), scene_(nullptr) {

}

IActor::~IActor() {
}

ActorId IActor::GetId() {
	return id_;
}

void IActor::SetId(ActorId id) {
	id_ = id;
}

void IActor::SetScene(Scene* scene) {
	scene_ = scene;
}

Scene& IActor::GetScene() {
	return *scene_;
}

void ActorImpl::HandleCommand(const CommandArgs& args) {
}

unique_ptr<QueryResult> ActorImpl::AnswerQuery(const QueryArgs& args) {
	return make_unique<QueryResult>(QueryType::EMPTY);
}

void ActorImpl::AddedToScene() {
}

void ActorImpl::PrepareToDie() {
}

void ActorImpl::DependencyDying(const ActorId& dying_id) {
}

void ActorImpl::RegisterDependency(const Target& target) {
	GetScene().RegisterDependency(Target(GetId()), target);
}

unique_ptr<QueryResult> ActorImpl::AskQuery(const Target& target, const QueryArgs& args) {
	return GetScene().AskQuery(target, args);
}

void ActorImpl::FailToHandleCommand(const CommandArgs& args) {
	// The ADDED_TO_SCENE command is expected to be allowed to be unhandled.
	if (args.Type() != CommandType::ADDED_TO_SCENE) {
		const auto& a = GetScene().GetCommandRegistry();
		std::cout <<
			"ACTOR ID: " << GetId().id_ << " OF CLASS: " << GetNameVirt() <<
			" HAS FAILED TO HANDLE COMMAND: " << std::hex << args.Type() << ", " <<
			a.LookupNameConst(args.Type()) <<  std::endl;
	}
}

string ActorImpl::GetName() {
	return "ActorImpl";
}

}  // game_scene