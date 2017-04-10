#pragma once

#include "stdafx.h"

#include "ActorId.h"
#include "CommandQueue.h"
#include "QueryResult.h"

namespace game_scene {
class Scene;
class QueryArgs;
class Target;

class IActor
{
public:
	IActor();
	virtual ~IActor();

	// Overridable behaviors
	virtual void HandleCommandVirt(const CommandArgs& args) { throw std::logic_error("Not Implemented"); };
	virtual unique_ptr<QueryResult> AnswerQueryVirt(const QueryArgs& args) { throw std::logic_error("Not Implemented"); };
	virtual void AddedToSceneVirt() { throw std::logic_error("Not Implemented"); };
	virtual void PrepareToDieVirt() { throw std::logic_error("Not Implemented"); };
	virtual void DependencyDyingVirt(const ActorId& dying_id) { throw std::logic_error("Not Implemented"); };
	virtual string GetNameVirt() const { throw std::logic_error("Not Implemented"); };

	ActorId GetId();
	void SetId(ActorId id);
	void SetScene(Scene* scene);
	Scene& GetScene();

private:
	Scene* scene_;
	ActorId id_;
};

template <typename Impl>
class ActorAdapter : public Impl {
public:
	ActorAdapter() {}
	ActorAdapter(Impl impl) : Impl(impl) {}

	void HandleCommandVirt(const CommandArgs& args) override;
	unique_ptr<QueryResult> AnswerQueryVirt(const QueryArgs& args) override;
	void AddedToSceneVirt() override;
	void PrepareToDieVirt() override;
	void DependencyDyingVirt(const ActorId& dying_id) override;
	string GetNameVirt() const override;
};

#define ADD_ACTOR_ADAPTER(CLASS_NAME) typedef game_scene::ActorAdapter< CLASS_NAME ## Impl > CLASS_NAME

class ActorImpl : public IActor
{
public:
	void HandleCommand(const CommandArgs& args);
	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args);
	void AddedToScene();
	void PrepareToDie();
	void DependencyDying(const ActorId& dying_id);
	static string GetName();

protected:
	void FailToHandleCommand(const CommandArgs& args);
	void RegisterDependency(const Target& target);
	unique_ptr<QueryResult> AskQuery(const Target& target, const QueryArgs& args);
};

template <typename Impl>
void ActorAdapter<Impl>::HandleCommandVirt(const CommandArgs& args) {
	Impl::HandleCommand(args);
}

template <typename Impl>
unique_ptr<QueryResult> ActorAdapter<Impl>::AnswerQueryVirt(const QueryArgs& args) {
	return Impl::AnswerQuery(args);
}

template <typename Impl>
void ActorAdapter<Impl>::AddedToSceneVirt() {
	Impl::AddedToScene();
}

template <typename Impl>
void ActorAdapter<Impl>::PrepareToDieVirt() {
	Impl::PrepareToDie();
}

template <typename Impl>
void ActorAdapter<Impl>::DependencyDyingVirt(const ActorId& dying_id) {
	Impl::DependencyDying(dying_id);
}

template <typename Impl>
string ActorAdapter<Impl>::GetNameVirt() const {
	return "ActorAdapter-" + Impl::GetName();
}

}  // game_scene