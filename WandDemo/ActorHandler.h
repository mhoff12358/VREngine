#pragma once

#include "stdafx.h"

#include "Actor.h"

class ActorHandler
{
public:
	ActorHandler(VRBackendBasics& graphics_objects);
	~ActorHandler();

	void LoadSceneFromLuaScript(const string& script_name);

	Actor* CreateActorFromLuaScript(const string& script_name, Identifier ident, ConstantBuffer* shader_settings = NULL);

	set<Lua::InteractableObject*>& LookupListeners(const string& listener_set_name);

	VRBackendBasics& graphics_objects_;
	InteractableCollection interactable_collection_;
	map<string, Actor*> actor_lookup_;
	map<string, set<Lua::InteractableObject*>> listeners_;
	Lua::Environment root_environment_;
	Lua::InteractableObject scene_interactable_;

public:
	// All the lua interaction callbacks

	// Arguments are <Actor* actor>
	int AddListener(lua_State* L);
	// Arguments are <Actor* actor>
	int RemoveListener(lua_State* L);
	// Arguments are <table representing a matrix to be constructed>
	//int Raycast(lua_State* L);
	// Arguments are <int resource_type, string resource_name>
	int LookupResource(lua_State* L);
	
	// Arguments are <Actor* actor>
	int AddActor(lua_State* L);
};
