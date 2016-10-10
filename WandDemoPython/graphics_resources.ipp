#pragma once

class_<ResourcePool, boost::noncopyable>("ResourcePool", no_init);
class_<EntityHandler, boost::noncopyable>("EntityHandler", no_init)
	.def("MutableCamera", &EntityHandler::MutableCamera, return_value_policy<reference_existing_object>());

class_<game_scene::actors::GraphicsResources, boost::noncopyable>("GraphicsResources", no_init)
	.def("GetResourcePool", &game_scene::actors::GraphicsResources::GetResourcePool, return_value_policy<reference_existing_object>())
	.def("GetEntityHandler", &game_scene::actors::GraphicsResources::GetEntityHandler, return_value_policy<reference_existing_object>());

class_<game_scene::QueryResultWrapped<game_scene::actors::GraphicsResources&>, bases<game_scene::QueryResult>, boost::noncopyable>("WrappedGraphicsResources", no_init)
	.def("GetGraphicsResources", &GetGraphicsResources, return_value_policy<reference_existing_object>());

class_<game_scene::GraphicsResourceQuery, boost::noncopyable>("GraphicsResourceQuery", no_init)
	.def_readonly("GRAPHICS_RESOURCE_REQUEST", &game_scene::GraphicsResourceQuery::GRAPHICS_RESOURCE_REQUEST);
