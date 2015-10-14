// LuaGameScripting.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Environment.h"
#include "InteractableObject.h"

#include <iostream>

int main()
{
	Lua::Environment lua_env(true);

	bool load_success = lua_env.RunFile("test.lua");
	
	Lua::InteractableObject game_obj = Lua::InteractableObject::LoadFromFile("dummy_interactable.lua");
	game_obj.CallLuaFunc("test_func");

    return 0;
}

