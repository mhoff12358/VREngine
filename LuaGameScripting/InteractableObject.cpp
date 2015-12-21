#include "stdafx.h"
#include "InteractableObject.h"

namespace Lua {
	InteractableObject::InteractableObject()
		: table_location_(0)
	{
	}

	InteractableObject::InteractableObject(Environment env)
		: env_(env), table_location_(env.CheckSizeOfStack())
	{
		assert(lua_type(env.L, -1) == LUA_TTABLE);
	}


	InteractableObject::~InteractableObject()
	{
	}

	bool InteractableObject::PrepareLuaFunc(const string& func_name) {
		if (!env_.GetFromTableToStack(func_name, table_location_)) {
			return false;
		}
		int found_type = env_.CheckTypeOfStack(Index(-1));
		if (found_type != LUA_TFUNCTION) {
			env_.RemoveFromStack();
			return false;
		}
		return true;
	}

	bool InteractableObject::CallLuaFunc(const string& func_name) {
		return PrepareLuaFunc(func_name) && env_.StoreToStack(table_location_) && env_.CallFunctionWithArgsOnStack<1>();
	}

	bool InteractableObject::AddCObjectMember(const string& func_name, void* object, CFunctionClosureId closure) {
		if (env_.GetFromTableToStack(string("callbacks"), table_location_) && env_.StoreToStack(object, closure)) {
			if (env_.StoreToTable(func_name, Lua::Index(env_.CheckSizeOfStack()), Lua::Index(env_.CheckSizeOfStack() - 1))) {
				env_.RemoveFromStack();
				env_.RemoveFromStack();
				return true;
			}
			env_.RemoveFromStack();
			env_.RemoveFromStack();
			env_.RemoveFromStack();
			return false;
		}
		return false;
	}

}  // namespace Lua