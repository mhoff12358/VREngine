#pragma once

#include "stdafx.h"

#include "Actor.h"

class ActorHandler
{
public:
	ActorHandler(VRBackendBasics& graphics_objects);
	~ActorHandler();

	Actor* CreateActorFromLuaScript(const string& script_name, Identifier ident, ConstantBuffer* shader_settings = NULL);

	VRBackendBasics& graphics_objects_;
	InteractableCollection interactable_collection_;
	map<string, Actor*> actor_lookup_;
	Lua::Environment root_environment_;
};
