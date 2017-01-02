#include "stdafx.h"
#include "GraphicsResourcesInterface.h"

#include "SceneSystem/GraphicsResources.h"
#include "VRBackend/EntityHandler.h"

boost::reference_wrapper<ID3D11Device> GetDeviceInterfaceReference(game_scene::actors::GraphicsResources& graphics_resources) {
	return boost::ref(*graphics_resources.GetDeviceInterface());
}

void EntityHandlerAddModelMutation(EntityHandler& entity_handler, string model_name, ModelMutation& mutation) {
	entity_handler.AddModelMutation(model_name, std::move(mutation));
}

void GraphicsResourcesInterface() {
	class_<ResourcePool, boost::noncopyable>("ResourcePool", no_init);
	class_<EntityHandler, boost::noncopyable>("EntityHandler", no_init)
		.def("MutableCamera", &EntityHandler::MutableCamera, return_value_policy<reference_existing_object>())
		.def("MutableLightSystem", &EntityHandler::MutableLightSystem, return_value_policy<reference_existing_object>())
		.def("AddModelMutation", &EntityHandlerAddModelMutation);
	class_<boost::reference_wrapper<ID3D11Device>>("ID3D11Device", no_init);

	class_<game_scene::actors::GraphicsResources, boost::noncopyable>("GraphicsResources", no_init)
		.def("GetResourcePool", &game_scene::actors::GraphicsResources::GetResourcePool, return_value_policy<reference_existing_object>())
		.def("GetEntityHandler", &game_scene::actors::GraphicsResources::GetEntityHandler, return_value_policy<reference_existing_object>())
		.def("GetDeviceInterface", &GetDeviceInterfaceReference)
		.def("GetGraphicsResources", &game_scene::actors::GraphicsResources::GetGraphicsResources, return_value_policy<reference_existing_object>())
		.staticmethod("GetGraphicsResources");

	class_<game_scene::QueryResultWrapped<game_scene::actors::GraphicsResources&>, bases<game_scene::QueryResult>, boost::noncopyable>("WrappedGraphicsResources", no_init)
		.def("GetGraphicsResources", &game_scene::QueryResultWrapped<game_scene::actors::GraphicsResources&>::GetData, return_value_policy<reference_existing_object>());

	class_<game_scene::GraphicsResourceQuery, boost::noncopyable>("GraphicsResourceQuery", no_init)
		.def_readonly("GRAPHICS_RESOURCE_REQUEST", &game_scene::GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST);
}