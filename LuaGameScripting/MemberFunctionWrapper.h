#pragma once

#include "stdafx.h"
#include "Lua52/lua.hpp"

namespace Lua {

	template <typename T, int(T::*lua_func_type)(lua_State*)>
	int MemberCallback(lua_State* L) {
		void* t_void = lua_touserdata(L, lua_upvalueindex(1));
		T* t = static_cast<T*>(t_void);
		return (t->*lua_func_type)(L);
	}

}  // namespace Lua