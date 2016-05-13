#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "CommandArgs.h"
#include "SpecializedCommandArgs.h"

// Includes from external code that is used for behavior.
#include "Camera.h"
#include "EntityHandler.h"
#include "ResourcePool.h"

namespace game_scene {
namespace actors {

class MovableCamera : public Shmactor {
public:
	MovableCamera(Camera* camera) : camera_(camera) {}

	void HandleCommand(const CommandArgs& args) override;
	void HandleInput(const commands::InputUpdate& args);

private:
	Camera* camera_;
};

class GraphicsResources : public Shmactor {
public:
	GraphicsResources(ResourcePool& resource_pool, EntityHandler& entity_handler)
		: resource_pool_(resource_pool), entity_handler_(entity_handler) {}

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) override;

	ResourcePool& resource_pool_;
	EntityHandler& entity_handler_;
};

class GraphicsEntity {

};

}  // actors
}  // game_scene
