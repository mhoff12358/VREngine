#pragma once

#include "Environment.h"

namespace Lua {

	class InteractableObject
	{
	public:
		explicit InteractableObject(Environment env);
		~InteractableObject();

		static InteractableObject LoadFromFile(const string& file_name);
		template<typename... Args>
		static InteractableObject LoadFromFile(const string& file_name, Args... args);

		bool PrepareLuaFunc(const string& func_name);

		bool CallLuaFunc(const string& func_name);
		template <typename... ArgumentTypes>
		bool CallLuaFunc(const string& func_name, ArgumentTypes... args);

		Environment env_;
	};


	template <typename... ArgumentTypes>
	bool InteractableObject::CallLuaFunc(const string& func_name, ArgumentTypes... args) {
		return PrepareLuaFunc(func_name) && env_.CallFunctionFromStackTop(args...);
	}

	template<typename... Args>
	static InteractableObject InteractableObject::LoadFromFile(const string& file_name, Args... args) {
		Environment lua_env(true);
		bool load_success = lua_env.RunFile(file_name);
		lua_env.CallGlobalFunction(string("create_cpp_interface"), args...);
		return InteractableObject(lua_env);
	}

}  // namespace Lua