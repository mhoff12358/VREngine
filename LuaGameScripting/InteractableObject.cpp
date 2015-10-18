#include "stdafx.h"
#include "InteractableObject.h"

namespace Lua {
	InteractableObject::InteractableObject() {
	}

	InteractableObject::InteractableObject(Environment env)
		: env_(env)
	{
		assert(lua_type(env.L, 1) == LUA_TTABLE);
		assert(lua_gettop(env.L) == 1);
	}


	InteractableObject::~InteractableObject()
	{
	}

	bool InteractableObject::PrepareLuaFunc(const string& func_name) {
		env_.GetFromTableToStack(func_name, Index(1));
		if (env_.CheckTypeOfStack(Index(-1)) != LUA_TFUNCTION) {
			env_.RemoveFromStack();
			return false;
		}
		return true;
	}

	bool InteractableObject::CallLuaFunc(const string& func_name) {
		return PrepareLuaFunc(func_name) && env_.StoreToStack(Index(1)) && env_.CallFunctionWithArgsOnStack<1>();
	}

}  // namespace Lua