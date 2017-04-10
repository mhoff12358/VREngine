#pragma once

#include "stdafx.h"
#include "Actor.h"
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

class GraphicsResourcesImpl;
typedef ActorAdapter<GraphicsResourcesImpl> GraphicsResources;
class GraphicsResourcesImpl : public ActorImpl {
public:
	GraphicsResourcesImpl(
		ResourcePool& resource_pool, EntityHandler& entity_handler, ID3D11Device* device_interface)
		: resource_pool_(resource_pool),
		  entity_handler_(entity_handler),
		  device_interface_(device_interface) {}

	unique_ptr<QueryResult> AnswerQuery(const QueryArgs& args);
	void HandleCommand(const CommandArgs& args);

	static string GetName() {
		return "GraphicsResourcesImpl-" + ActorImpl::GetName();
	}

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
ADD_ACTOR_ADAPTER(GraphicsResources);

}  // actors
}  // game_scene
