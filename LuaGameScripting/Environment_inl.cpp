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
	bool Environment::StoreArrayToStack(T const* stored_value, size_t array_length) {
		lua_newtable(L);
		bool success = true;
		for (int i = 0; i < array_length; i++) {
			success = success && StoreToTable(i + 1, stored_value[i], stack_top);
		}
		return success;
	}

	template <typename T>
	bool Environment::LoadGlobal(const string& var_name, T* loaded_value) {
		return GetGlobalToStack(var_name) && LoadFromStack(loaded_value);
	}

	template <typename T>
	bool Environment::StoreGlobal(const string& var_name, const T& stored_value) {
		return StoreToStack(stored_value) && SetGlobalFromStack(var_name);
	}

	template<std::size_t I, std::size_t J, typename... Args>
	typename std::enable_if<I == J, bool>::type Environment::LoadTupleElement(tuple<Args...>& t, Index stack_position) {
		return true;
	}

	template<std::size_t I, std::size_t J, typename... Args>
	typename std::enable_if<I != J, bool>::type Environment::LoadTupleElement(tuple<Args...>& t, Index stack_position) {
		std::tuple_element<I, tuple<Args...>>::type val;
		if (!LoadFromTable(I+1, &val)) {
			return false;
		}
		std::get<I>(t) = val;
		return LoadTupleElement<I + 1, J>(t, stack_position);
	}

	template <typename... Args>
	bool Environment::PeekFromStack(tuple<Args...>* loaded_value, Index stack_position) {
		std::size_t s = sizeof...(Args);
		return LoadTupleElement<0, sizeof...(Args), Args...>(*loaded_value, stack_position);
	}

	template <typename S, size_t N>
	bool Environment::PeekFromStack(array<S, N>* loaded_value, Index stack_position) {
		return PeekArrayFromStack(loaded_value.data(), stack_position, N);
	}

	template <typename S, size_t N>
	bool Environment::StoreToStack(const array<S, N>& stored_value) {
		return StoreArrayToStack(stored_value.data(), N);
	}

	template <typename T>
	bool Environment::PeekFromStack(vector<T>* loaded_value, Index stack_position) {
		int num_elements_to_copy = GetArrayLength(stack_position);
		loaded_value->resize(num_elements_to_copy);
		bool success = true;
		for (int i = 0; i < num_elements_to_copy; i++) {
			success = success && LoadFromTable(i + 1, loaded_value->data() + i, stack_position);
		}
		return success;
	}

	template <typename T>
	bool Environment::StoreToStack(const vector<T>& stored_value) {
		StoreToStack(stored_value.data(), stored_value.size());
	}

	template <typename K, typename V>
	bool Environment::LoadFromTable(const K& key, V* loaded_value, Index table_location) {
		bool success = GetFromTableToStack(key, table_location);
		if (success) {
			success = LoadFromStack(loaded_value);
			if (!success) {
				RemoveFromStack();
			}
		}
		return success;
	}

	template <typename K, typename V>
	bool Environment::LoadArrayFromTable(const K& key, V* loaded_value, Index table_location, int max_num_loaded) {
		return GetFromTableToStack(key, table_location) && LoadArrayFromStack(loaded_value, stack_top, max_num_loaded);
	}

	template <typename K, typename V>
	bool Environment::StoreToTable(const K& key, const V& stored_value, Index table_location) {
		bool success = StoreToStack(key) && StoreToStack(stored_value);
		if (success) {
			lua_settable(L, table_location.Offset(2).index_);
		}
		return success;
	}

	template <typename T>
	bool Environment::LoadFromStack(T* loaded_value, Index stack_position) {
		bool success = PeekFromStack(loaded_value, stack_position);
		if (!success) {
			return false;
		}
		lua_remove(L, stack_position.index_);
		return true;
	}

	template <typename T>
	bool Environment::PeekArrayFromStack(T* loaded_value, Index stack_position, int max_num_loaded) {
		int num_elements_to_copy = GetArrayLength(stack_position);
		if (max_num_loaded >= 0) {
			num_elements_to_copy = min(max_num_loaded, num_elements_to_copy);
		}
		bool success = true;
		for (int i = 0; i < num_elements_to_copy; i++) {
			success = success && LoadFromTable(i + 1, loaded_value + i, stack_position);
		}
		return success;
	}

	template <typename T>
	bool Environment::LoadArrayFromStack(T* loaded_value, Index stack_position, int max_num_loaded) {
		bool success = PeekArrayFromStack(loaded_value, stack_position, max_num_loaded);
		lua_remove(L, stack_position.index_);
		return success;
	}

	template <typename K, typename V>
	bool Environment::IterateOverTable(K* key, V* value, bool* succesful, Index table_location) {
		bool finished = lua_next(L, table_location.Offset(1).index_) == 0;
		if (finished) return false;
		if (succesful == NULL) {
			return LoadFromStack(value) && PeekFromStack(key);
		}
		*succesful = LoadFromStack(value) && PeekFromStack(key);
		return *succesful;
	}

	template <typename K>
	bool Environment::IterateOverTableLeaveValue(K* key, bool* succesful, Index table_location) {
		// Pop the key left from last time
		RemoveFromStack();
		bool finished = lua_next(L, table_location.Offset(1).index_) == 0;
		if (finished) return false;
		if (succesful == NULL) {
			return PeekFromStack(key, Index(-2));
		}
		*succesful = PeekFromStack(key, Index(-2));
		return *succesful;
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
		int call_status = lua_pcall(L, num_args_on_stack, LUA_MULTRET, 0);
		if (call_status != LUA_OK) {
			PrintStack("Call function error");
		}
		return call_status == LUA_OK;
	}

	template <typename... Args>
	bool Environment::CallGlobalFunction(const string& function_name, Args... args) {
		return GetGlobalToStack(function_name) && CallFunctionOnStackTop(args...);
	}

}  // namespace Lua