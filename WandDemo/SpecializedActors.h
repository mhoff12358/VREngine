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

enum class GraphicsResourceId {
	RESOURCE_POOL,
	ENTITY_HANDLER,
	DEVICE_INTERFACE,
};

class GraphicsResources : public Shmactor {
public:
	GraphicsResources(
		ResourcePool& resource_pool, EntityHandler& entity_handler, ID3D11Device* device_interface)
		: resource_pool_(resource_pool),
		  entity_handler_(entity_handler),
		  device_interface_(device_interface) {}

	void AddedToScene() override;
	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) override;
	void HandleCommand(const CommandArgs& args) override;

	void RequireResource(ResourceIdent resource_ident);

	ResourcePool& resource_pool_;
	EntityHandler& entity_handler_;
	ID3D11Device* device_interface_;
};

}  // actors
}  // game_scene
