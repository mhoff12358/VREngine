#pragma once

#include "Environment.h"

namespace Lua {

	class InteractableObject
	{
	public:
		explicit InteractableObject(Environment env, Index interface_table_index);
		~InteractableObject();

		bool PrepareLuaFunc(const string& func_name);

		bool CallLuaFunc(const string& func_name);
		template <typename... ArgumentTypes>
		bool CallLuaFunc(const string& func_name, ArgumentTypes... args);

		Environment env_;
		Index interface_table_index_;
	};


	template <typename... ArgumentTypes>
	bool InteractableObject::CallLuaFunc(const string& func_name, ArgumentTypes... args) {
		return PrepareLuaFunc(func_name) && env_.CallFunctionFromStackTop(args...);
	}

}  // namespace Lua