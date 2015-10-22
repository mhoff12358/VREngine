#include "stdafx.h"
#include "Environment.h"

#include <iostream>

namespace Lua {

	Index Environment::stack_top = Index(-1);

	Environment::Environment() : L(NULL) {}

	Environment::Environment(bool initialize_libs) {
		Initialize(initialize_libs);
	}

	Environment::Environment(lua_State* injected_state) {
		L = injected_state;
	}

	void Environment::Initialize(bool initialize_libs) {
		L = luaL_newstate();
		if (initialize_libs) {
			luaL_openlibs(L);
		}
	}

	bool Environment::RunFile(const string& file_name) {
		if (luaL_loadfile(L, file_name.c_str())) {
			auto c = lua_tostring(L, -1);
			return false;
		}
		if (lua_pcall(L, 0, 0, 0)) {
			return false;
		}
		return true;
	}

	void Environment::ClearStack() {
		lua_settop(L, 0);
	}

	void Environment::RemoveFromStack(Index stack_position) {
		lua_remove(L, stack_position.index_);
	}

	int Environment::CheckTypeOfStack(Index stack_position) {
		return lua_type(L, stack_position.index_);
	}

	int Environment::CheckSizeOfStack() {
		return lua_gettop(L);
	}

	int Environment::GetArrayLength(Index stack_position) {
		if (!lua_istable(L, stack_position.index_)) {
			return -1;
		}
		lua_len(L, stack_position.index_);
		int length;
		LoadFromStack(&length);
		return length;
	}

	bool Environment::StoreToStack() {
		lua_pushnil(L);
		return true;
	}

	template <>
	bool Environment::StoreToStack<void*>(void* const& stored_value) {
		lua_pushlightuserdata(L, stored_value);
		return true;
	}

	template <>
	bool Environment::PeekFromStack<void*>(void** loaded_value, Index stack_position) {
		if (!lua_islightuserdata(L, stack_position.index_)) {
			return false;
		}
		*loaded_value = lua_touserdata(L, stack_position.index_);
		return true;
	}

	template <>
	bool Environment::StoreToStack<lua_CFunction>(const lua_CFunction& stored_value) {
		lua_pushcfunction(L, stored_value);
		return true;
	}

	template <>
	bool Environment::StoreToStack<CFunctionClosureId>(const CFunctionClosureId& stored_value) {
		lua_pushcclosure(L, stored_value.function, stored_value.num_args);
		return true;
	}

	template <>
	bool Environment::StoreToStack<bool>(const bool& stored_value) {
		lua_pushboolean(L, stored_value);
		return true;
	}

	template <>
	bool Environment::PeekFromStack<bool>(bool* loaded_value, Index stack_position) {
		if (!lua_isboolean(L, stack_position.index_)) {
			return false;
		}
		*loaded_value = lua_toboolean(L, stack_position.index_);
		return true;
	}

	template <>
	bool Environment::StoreToStack<int>(const int& stored_value) {
		lua_pushinteger(L, stored_value);
		return true;
	}

	template <>
	bool Environment::PeekFromStack<int>(int* loaded_value, Index stack_position) {
		if (!lua_isnumber(L, stack_position.index_)) {
			return false;
		}
		*loaded_value = lua_tointeger(L, stack_position.index_);
		return true;
	}

	template <>
	bool Environment::PeekFromStack<float>(float* loaded_value, Index stack_position) {
		if (!lua_isnumber(L, stack_position.index_)) {
			return false;
		}
		*loaded_value = (float)lua_tonumber(L, stack_position.index_);
		return true;
	}

	template <>
	bool Environment::StoreToStack<float>(const float& stored_value) {
		lua_pushnumber(L, stored_value);
		return true;
	}

	template <>
	bool Environment::PeekFromStack<double>(double* loaded_value, Index stack_position) {
		if (!lua_isnumber(L, stack_position.index_)) {
			return false;
		}
		*loaded_value = (double)lua_tonumber(L, stack_position.index_);
		return true;
	}

	template <>
	bool Environment::StoreToStack<double>(const double& stored_value) {
		lua_pushnumber(L, stored_value);
		return true;
	}

	template <>
	bool Environment::PeekFromStack<unsigned int>(unsigned int* loaded_value, Index stack_position) {
		if (!lua_isnumber(L, stack_position.index_)) {
			return false;
		}
		*loaded_value = (unsigned int)lua_tonumber(L, stack_position.index_);
		return true;
	}

	template <>
	bool Environment::StoreToStack<unsigned int>(const unsigned int& stored_value) {
		lua_pushnumber(L, stored_value);
		return true;
	}

	template <>
	bool Environment::PeekFromStack<std::size_t>(std::size_t* loaded_value, Index stack_position) {
		if (!lua_isnumber(L, stack_position.index_)) {
			return false;
		}
		*loaded_value = (std::size_t)lua_tonumber(L, stack_position.index_);
		return true;
	}

	template <>
	bool Environment::StoreToStack<std::size_t>(const std::size_t& stored_value) {
		lua_pushnumber(L, stored_value);
		return true;
	}

	template <>
	bool Environment::PeekFromStack<string>(string* loaded_value, Index stack_position) {
		if (!lua_isstring(L, stack_position.index_)) {
			return false;
		}
		*loaded_value = string(lua_tostring(L, stack_position.index_));
		return true;
	}

	template <>
	bool Environment::StoreToStack<string>(const string& stored_value) {
		lua_pushstring(L, stored_value.c_str());
		return true;
	}

	template <>
	bool Environment::StoreToStack<Index>(const Index& stored_value) {
		lua_pushvalue(L, stored_value.index_);
		return true;
	}

	bool Environment::GetGlobalToStack(const string& var_name) {
		lua_getglobal(L, var_name.c_str());
		return true;
	}

	bool Environment::SetGlobalFromStack(const string& var_name) {
		lua_setglobal(L, var_name.c_str());
		return true;
	}

	bool Environment::BeginToIterateOverTable() {
		return StoreToStack();
	
	}
	bool Environment::BeginToIterateOverTableLeaveValue() {
		return StoreToStack() && StoreToStack();
	}

	bool Environment::BeginToIterateOverTableLeaveKey() {
		return StoreToStack() && StoreToStack();
	}

	bool Environment::IterateOverTableLeaveKey(bool* successful, Index table_location) {
		RemoveFromStack();
		return lua_next(L, table_location.Offset(1).index_) != 0;
	}

	void Environment::PrintStack(const string& debug_msg) {
		int stack_size = lua_gettop(L);

		std::cout << "Printing stack for: " << debug_msg << std::endl;
		std::cout << "Stack size : " << stack_size << std::endl;
		for (int index = 1; index <= stack_size; ++index) {
			int type = lua_type(L, index);
			const char* typename_str = lua_typename(L, type);
			std::cout << "Index " << index << "\tType " << string(typename_str);
			if (type == LUA_TNUMBER) {
				double value;
				PeekFromStack(&value, Index(index));
				std::cout << "\tValue " << value;
			}
			else if (type == LUA_TSTRING) {
				string value;
				PeekFromStack(&value, Index(index));
				std::cout << "\tValue " << value;
			}
			else if (type == LUA_TLIGHTUSERDATA) {
				void* value;
				PeekFromStack(&value, Index(index));
				std::cout << "\tValue " << value;
			}
			std::cout << std::endl;
		}
	}

	bool Environment::CallGlobalFunction(const string& function_name) {
		return GetGlobalToStack(function_name) && CallFunctionOnStackTop();
	}

	bool Environment::CallFunctionOnStackTop() {
		return CallFunctionWithArgsOnStack<0>();
	}


}  // namespace Lua