#include "stdafx.h"
/*#include "ActorHandler.h"


ActorHandler::ActorHandler(VRBackendBasics& graphics_objects)
	: graphics_objects_(graphics_objects), interactable_collection_(50), root_environment_(true)
{
	//lua_newtable(root_environment_.L);
	//root_environment_.SetGlobalFromStack(string("actor_interfaces"));
}


ActorHandler::~ActorHandler()
{
}

void ActorHandler::LoadSceneFromLuaScript(const string& script_name) {
	Lua::Environment new_environment(lua_newthread(root_environment_.L));
	new_environment.RunFile(script_name);
	if (new_environment.CheckSizeOfStack() != 0) {
		new_environment.PrintStack("ERROR RUNNING FILE");
	}

	if (!new_environment.CallGlobalFunction(string("create_scene"))) {
		new_environment.PrintStack("Error loading lua script scene: " + script_name);
		return;
	}
	scene_interactable_ = Lua::InteractableObject(Lua::Environment(new_environment));

	scene_interactable_.env_.StoreToTable(string("cpp_interface"), (void*)&scene_interactable_);
	scene_interactable_.AddCObjectMember("add_actor", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::AddActor >, 1 }));
	scene_interactable_.AddCObjectMember("add_listener", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::AddListener >, 1 }));
	scene_interactable_.AddCObjectMember("remove_listener", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::RemoveListener >, 1 }));

	scene_interactable_.CallLuaFunc("initialize");
}

Actor* ActorHandler::CreateActorFromLuaScript(const string& script_name, Identifier ident, ConstantBuffer* shader_settings) {
	Actor* new_actor = new Actor(ident, *this, *graphics_objects_.resource_pool, *graphics_objects_.entity_handler);
	lua_State* new_lua_thread = lua_newthread(root_environment_.L);
	new_actor->InitializeFromLuaScript(new_lua_thread, graphics_objects_, script_name, ident);
	actor_lookup_[ident.GetId()] = new_actor;

	new_actor->lua_interface_.AddCObjectMember("add_listener", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::AddListener >, 1 }));
	new_actor->lua_interface_.AddCObjectMember("remove_listener", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::RemoveListener >, 1 }));
	new_actor->lua_interface_.AddCObjectMember("lookup_resource", this,
		Lua::CFunctionClosureId({ (lua_CFunction)&Lua::MemberCallback < ActorHandler, &ActorHandler::LookupResource >, 1 }));
	
	return new_actor;
}

set<Lua::InteractableObject*>& ActorHandler::LookupListeners(const string& listener_set_name) {
	return listeners_[listener_set_name];
}

int ActorHandler::AddListener(lua_State* L) {
	Lua::Environment env(L);
	string listener_set_name;
	assert(env.LoadFromStack(&listener_set_name, Lua::Index(1)));
	void* actor_ptr = NULL;
	assert(env.LoadFromStack(&actor_ptr, Lua::Index(1)));
	LookupListeners(listener_set_name).insert((Lua::InteractableObject*)actor_ptr);
	return 0;
}

int ActorHandler::RemoveListener(lua_State* L) {
	Lua::Environment env(L);
	string listener_set_name;
	assert(env.LoadFromStack(&listener_set_name, Lua::Index(1)));
	void* actor_ptr = NULL;
	assert(env.LoadFromStack(&actor_ptr, Lua::Index(1)));
	LookupListeners(listener_set_name).erase((Lua::InteractableObject*)actor_ptr);
	return 0;
}

int ActorHandler::AddActor(lua_State* L) {
	Lua::Environment env(L);

	string script_name;
	string actor_name;
	env.LoadFromStack(&script_name, Lua::Index(1));
	env.LoadFromStack(&actor_name, Lua::Index(1));

	Actor* new_actor = CreateActorFromLuaScript(script_name, Identifier(actor_name), NULL);

	// Duplicate the interface so it can be xmove'd over to the other thread.
	new_actor->lua_interface_.env_.StoreToStack(Lua::Index(1));
	env.StoreToStack(Lua::EnvironmentTop{ &new_actor->lua_interface_.env_, 1 });

	if (env.CheckSizeOfStack() != 1) {
		env.PrintStack();
	}

	return 1;
}

int ActorHandler::LookupResource(lua_State* L) {
	Lua::Environment env(L);

	int resource_type;
	string resource_name;
	string resource_request;
	env.LoadFromStack(&resource_type, Lua::Index(1));
	env.LoadFromStack(&resource_name, Lua::Index(1));
	env.LoadFromStack(&resource_request, Lua::Index(1));

	if (resource_request == "data") {
		vector<float> resource_data = graphics_objects_.resource_pool->AccessDataFromResource(ResourceIdent(static_cast<ResourceIdent::ResourceType>(resource_type), resource_name));
		env.StoreToStack(resource_data);
		return 1;
	}
	else if (resource_request == "size") {
		switch (resource_type) {
		case ResourceIdent::TEXTURE:
			env.StoreToStack(graphics_objects_.resource_pool->LoadExistingTexture(resource_name).GetSize());
			return 1;
		default:
			return 0;
		}
	}
}

/*
int ActorHandler::Raycast(lua_State* L) {
	Lua::Environment env(L);

	tuple<Identifier, Actor*, float> intersected_object = interactable_collection_.GetClosestLookedAt(LuaTableToMatrix(env));
	env.StoreToStack(std::get<0>(intersected_object).GetId());
	env.StoreToStack(std::get<2>(intersected_object));

	return 2;
}*/