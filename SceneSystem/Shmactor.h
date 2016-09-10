#pragma once

#include "stdafx.h"
#include "CommandArgs.h"
#include "QueryArgs.h"
#include "QueryResult.h"
#include "Target.h"
#include "CommandQueue.h"
#include "ActorId.h"

namespace game_scene {
class Scene;

class DLLSTUFF Shmactor
{
public:
	Shmactor();
	virtual ~Shmactor();

	// Overridable behaviors
	virtual void HandleCommand(const CommandArgs& args);
	virtual unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args);
	virtual void AddedToScene();
	virtual void PrepareToDie();
	virtual void DependencyDying(const ActorId& dying_id);

	void FailToHandleCommand(const CommandArgs& args);

	void SetId(ActorId id);

	static void SetScene(Scene* scene);

protected:
	void RegisterDependency(const Target& target);
	CommandQueueLocation MakeCommandAfter(CommandQueueLocation location, const Target& target, unique_ptr<CommandArgs> args);
	unique_ptr<QueryResult> AskQuery(const Target& target, unique_ptr<QueryArgs> args);

	ActorId id_;

	static Scene* scene_;
};
}  // game_scene