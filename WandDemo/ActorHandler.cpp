#include "stdafx.h"
#include "ActorHandler.h"


ActorHandler::ActorHandler(VRBackendBasics& graphics_objects)
	: graphics_objects_(graphics_objects), interactable_collection_(50), root_environment_(true)
{
	lua_newtable(root_environment_.L);
	root_environment_.SetGlobalFromStack(string("actor_interfaces"));
}


ActorHandler::~ActorHandler()
{
}

Actor* ActorHandler::CreateActorFromLuaScript(const string& script_name, Identifier ident, ConstantBuffer* shader_settings) {
	Actor* new_actor = new Actor(ident, *graphics_objects_.resource_pool, *graphics_objects_.entity_handler);
	lua_State* new_lua_thread = lua_newthread(root_environment_.L);
	new_actor->InitializeFromLuaScript(new_lua_thread, script_name, graphics_objects_, interactable_collection_, this, shader_settings);
	actor_lookup_[ident.GetId()] = new_actor;

	new_actor->lua_interface_.CallLuaFunc("initialize");

	return new_actor;
}
