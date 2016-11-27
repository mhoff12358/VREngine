#pragma once

#include "stdafx.h"

#include "ActorId.h"
#include "CommandQueue.h"
#include "QueryResult.h"

namespace game_scene {
class Scene;
class QueryArgs;
class Target;

class Actor
{
public:
	Actor();
	Actor(const Actor& other) = delete;
	Actor operator=(const Actor& other) = delete;
	virtual ~Actor();

	// Overridable behaviors
	virtual void HandleCommand(const CommandArgs& args);
	virtual unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args);
	virtual void AddedToScene();
	virtual void PrepareToDie();
	virtual void DependencyDying(const ActorId& dying_id);

	void FailToHandleCommand(const CommandArgs& args);

	void SetId(ActorId id);
	void SetScene(Scene* scene);
	Scene& GetScene();

protected:
	void RegisterDependency(const Target& target);
	unique_ptr<QueryResult> AskQuery(const Target& target, const QueryArgs& args);

	Scene* scene_;
	ActorId id_;
};
}  // game_scene