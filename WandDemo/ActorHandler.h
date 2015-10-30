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

	VRBackendBasics& graphics_objects_;
	InteractableCollection interactable_collection_;
	map<string, Actor*> actor_lookup_;
	set<Actor*> mouse_motion_listeners_;
	set<Actor*> update_tick_listeners_;
	Lua::Environment root_environment_;

public:
	// All the lua interaction callbacks

	// Arguments are <Actor* actor>
	int AddMouseMovementListener(lua_State* L);
	// Arguments are <Actor* actor>
	int RemoveMouseMovementListener(lua_State* L);
	int AddUpdateTickListener(lua_State* L);
	// Arguments are <Actor* actor>
	int RemoveUpdateTickListener(lua_State* L);

	// Arguments are <Actor* actor>
	int AddActor(lua_State* L);
};
