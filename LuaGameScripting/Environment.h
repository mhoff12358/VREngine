#pragma once

#include "stdafx.h"
#include "Lua52/lua.hpp"
#include "Index.h"

namespace Lua {

	class Environment {
	public:
		static Index stack_top;

		Environment();
		explicit Environment(bool initialize_libs);
		explicit Environment(lua_State* injected_state);

		void Initialize(bool initialize_libs);
		bool RunFile(const string& file_name);

		void PrintStack(const string& debug_msg = "");

		void ClearStack();
		void RemoveFromStack(Index stack_position = stack_top);
		int CheckTypeOfStack(Index stack_position = stack_top);
		int CheckSizeOfStack();
		int GetArrayLength(Index stack_position = stack_top);

		bool GetGlobalToStack(const string& var_name);
		bool SetGlobalFromStack(const string& var_name);

		template <typename K>
		bool GetFromTableToStack(const K& key, Index table_location = stack_top);

		template <typename T>
		bool LoadFromStack(T* loaded_value, Index stack_position = stack_top);
		template <typename T>
		bool LoadArrayFromStack(T* loaded_value, Index stack_position = stack_top, int max_num_loaded = -1);
		//template <typename... value_types>
		//bool LoadFromStack(tuple<value_types...>* loaded_values, Index stack_position_start = -1, int num_values_to_load = 1);
		template <typename T>
		bool PeekFromStack(T* loaded_value, Index stack_position = stack_top);
		template <typename T>
		bool PeekArrayFromStack(T* loaded_value, Index stack_position = stack_top, int max_num_loaded = -1);
		bool StoreToStack();
		template <typename T>
		bool StoreToStack(const T& stored_value);
		//template <typename T>
		//bool StoreToStack(const T* stored_value);
		template <typename T, typename... Args>
		bool StoreToStack(const T& stored_value, Args... args);
		//template <typename T, typename... Args>
		//bool StoreToStack(const T* stored_value, Args... args);

		template <typename T>
		bool LoadGlobal(const string& var_name, T* loaded_value);
		template <typename T>
		bool StoreGlobal(const string& var_name, const T& stored_value);

		template <typename K, typename V>
		bool LoadFromTable(const K& key, V* loaded_value, Index table_location = stack_top);
		template <typename K, typename V>
		bool LoadArrayFromTable(const K& key, V* loaded_value, Index table_location = stack_top, int max_num_loaded = -1);
		template <typename K, typename V>
		bool StoreToTable(const K& key, const V& stored_value, Index table_location = stack_top);


		bool BeginToIterateOverTable();
		bool BeginToIterateOverTableLeaveValue();
		bool BeginToIterateOverTableLeaveKey();
		// Note that the table location index is the location after the topmost element is popped.
		// Returns 
		template <typename K, typename V>
		bool IterateOverTable(K* key, V* value, bool* successful, Index table_location = stack_top);
		template <typename K>
		bool IterateOverTableLeaveValue(K* key, bool* successful, Index table_location = stack_top);
		bool IterateOverTableLeaveKey(bool* successful, Index table_location = stack_top);

		bool CallGlobalFunction(const string& function_name);
		template <typename... Args>
		bool CallGlobalFunction(const string& function_name, Args... args);

		bool CallFunctionOnStackTop();
		template <typename... Args>
		bool CallFunctionOnStackTop(Args... args);
		template <int num_args_on_stack, typename T, typename... Args>
		bool CallFunctionWithArgsOnStack(const T& first_arg, Args... args);
		template <int num_args_on_stack, typename T>
		bool CallFunctionWithArgsOnStack(const T& arg);
		template <int num_args_on_stack>
		bool CallFunctionWithArgsOnStack();

		lua_State* L;

	private:

	};

}  // namespace Lua

#include "Environment_inl.cpp"