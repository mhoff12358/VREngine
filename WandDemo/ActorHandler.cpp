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

void ActorHandler::LoadSceneFromLuaScript(const string& script_name) {
	int initial_stack_size = root_environment_.CheckSizeOfStack();
	root_environment_.RunFile(script_name);
	if (root_environment_.CheckSizeOfStack() != initial_stack_size) {
		root_environment_.PrintStack("ERROR RUNNING FILE");
	}

	if (!root_environment_.CallGlobalFunction(string("create_scene"))) {
		root_environment_.PrintStack("Error loading lua script scene: " + script_name);
		return;
	}

	Lua::InteractableObject lua_interactable(root_environment_);
	lua_interactable.AddCObjectMember("add_actor", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::AddActor >, 1 }));

	root_environment_.PrintStack("root environment");
	lua_interactable.CallLuaFunc("initialize");
	root_environment_.RemoveFromStack();
}

Actor* ActorHandler::CreateActorFromLuaScript(const string& script_name, Identifier ident, ConstantBuffer* shader_settings) {
	Actor* new_actor = new Actor(ident, *graphics_objects_.resource_pool, *graphics_objects_.entity_handler);
	lua_State* new_lua_thread = lua_newthread(root_environment_.L);
	new_actor->InitializeFromLuaScript(new_lua_thread, script_name, graphics_objects_, interactable_collection_, this, shader_settings);
	actor_lookup_[ident.GetId()] = new_actor;

	new_actor->lua_interface_.AddCObjectMember("add_mouse_movement_listener", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::AddMouseMovementListener >, 1 }));
	new_actor->lua_interface_.AddCObjectMember("remove_mouse_movement_listener", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::RemoveMouseMovementListener >, 1 }));
	new_actor->lua_interface_.AddCObjectMember("add_update_tick_listener", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::AddUpdateTickListener >, 1 }));
	new_actor->lua_interface_.AddCObjectMember("remove_update_tick_listener", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::RemoveUpdateTickListener >, 1 }));
	new_actor->lua_interface_.CallLuaFunc("initialize");

	return new_actor;
}

int ActorHandler::AddMouseMovementListener(lua_State* L) {
	Lua::Environment env(L);
	void* actor_ptr;
	assert(env.LoadFromStack(&actor_ptr, Lua::Index(1)));
	mouse_motion_listeners_.insert((Actor*)actor_ptr);
	return 0;
}

int ActorHandler::RemoveMouseMovementListener(lua_State* L) {
	Lua::Environment env(L);
	void* actor_ptr;
	assert(env.LoadFromStack(&actor_ptr, Lua::Index(1)));
	mouse_motion_listeners_.erase((Actor*)actor_ptr);
	return 0;
}

int ActorHandler::AddUpdateTickListener(lua_State* L) {
	Lua::Environment env(L);
	void* actor_ptr;
	assert(env.LoadFromStack(&actor_ptr, Lua::Index(1)));
	update_tick_listeners_.insert((Actor*)actor_ptr);
	return 0;
}

int ActorHandler::RemoveUpdateTickListener(lua_State* L) {
	Lua::Environment env(L);
	void* actor_ptr;
	assert(env.LoadFromStack(&actor_ptr, Lua::Index(1)));
	update_tick_listeners_.erase((Actor*)actor_ptr);
	return 0;
}

int ActorHandler::AddActor(lua_State* L) {
	Lua::Environment env(L);

	string script_name;
	string actor_name;
	env.LoadFromStack(&script_name, Lua::Index(1));
	env.LoadFromStack(&actor_name, Lua::Index(1));

	CreateActorFromLuaScript(script_name, Identifier(actor_name), NULL);

	return 0;
}