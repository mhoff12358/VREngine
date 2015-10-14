#pragma once

namespace Lua {

	template <typename K>
	bool Environment::GetFromTableToStack(const K& key, Index table_location) {
		bool success = StoreToStack(key);
		if (!success) return false;
		lua_gettable(L, table_location.Offset(1).index_);
		return success;
	}

	template <typename T, typename... Args>
	bool Environment::StoreToStack(const T& stored_value, Args... args) {
		return StoreToStack(stored_value) && StoreToStack(args...);
	}

	template <typename T>
	bool Environment::LoadGlobal(const string& var_name, T* loaded_value) {

		return GetGlobalToStack(var_name) && LoadFromStack(loaded_value);
	}

	template <typename T>
	bool Environment::StoreGlobal(const string& var_name, const T& stored_value) {
		return StoreToStack(stored_value) && SetGlobalFromStack(var_name);
	}

	template <typename K, typename V>
	bool Environment::LoadFromTable(const K& key, V* loaded_value, Index table_location) {
		GetFromTableToStack(key, table_location);
		return LoadFromStack(loaded_value);
	}

	template <typename K, typename V>
	bool Environment::StoreToTable(const K& key, const V& stored_value, Index table_location) {
		StoreToStack(stored_value);
		return StoreToTableFromStack(key, table_location);
	}

	template <typename T>
	bool Environment::LoadFromStack(T* loaded_value, Index stack_position) {
		bool success = PeekFromStack(loaded_value, stack_position);
		if (!success) {
			return false;
		}
		lua_remove(L, stack_position);
		return true;
	}

	template <typename... Args>
	bool Environment::CallFunctionOnStackTop(Args... args) {
		return CallFunctionWithArgsOnStack<0>(args...);
	}

	template <int num_args_on_stack, typename T, typename... Args>
	bool Environment::CallFunctionWithArgsOnStack(const T& first_arg, Args... args) {
		return StoreToStack(first_arg) && CallFunctionWithArgsOnStack<num_args_on_stack + 1>(args...);
	}

	template <int num_args_on_stack, typename T>
	bool Environment::CallFunctionWithArgsOnStack(const T& arg) {
		return StoreToStack(arg) && CallFunctionWithArgsOnStack<num_args_on_stack + 1>();
	}

	template <int num_args_on_stack>
	bool Environment::CallFunctionWithArgsOnStack() {
		PrintStack();
		return lua_pcall(L, num_args_on_stack, LUA_MULTRET, 0) == LUA_OK;
	}

	template <typename... Args>
	bool Environment::CallGlobalFunction(const string& function_name, Args... args) {
		return GetGlobalToStack(function_name) && CallFunctionOnStackTop(args...);
	}

}  // namespace Lua