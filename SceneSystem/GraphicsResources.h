#pragma once

#include "stdafx.h"
#include "Shmactor.h"
#include "CommandArgs.h"

#include "VRBackend/ResourcePool.h"

class ResourcePool;
class EntityHandler;
struct ID3D11Device;

namespace game_scene {

class GraphicsResourceQuery {
public:
	DECLARE_QUERY(GraphicsResourceQuery, GRAPHICS_RESOURCE_REQUEST);
};

namespace actors {

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

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args) override;
	void HandleCommand(const CommandArgs& args) override;

	void RequireResource(ResourceIdent resource_ident);
	static GraphicsResources& GetGraphicsResources(Scene* scene);

	ResourcePool& GetResourcePool() { return resource_pool_; }
	EntityHandler& GetEntityHandler() { return entity_handler_; }
	ID3D11Device* GetDeviceInterface() { return device_interface_; }
	ID3D11Device& GetDeviceInterfaceRef() { return *device_interface_; }

	ResourcePool& resource_pool_;
	EntityHandler& entity_handler_;
	ID3D11Device* device_interface_;
};

}  // actors
}  // game_scene
