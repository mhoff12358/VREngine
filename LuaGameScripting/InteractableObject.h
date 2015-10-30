#pragma once

#include "Environment.h"

namespace Lua {

	class InteractableObject
	{
	public:
		InteractableObject();
		explicit InteractableObject(Environment env);
		~InteractableObject();

		bool PrepareLuaFunc(const string& func_name);

		bool CallLuaFunc(const string& func_name);
		template <typename... ArgumentTypes>
		bool CallLuaFunc(const string& func_name, ArgumentTypes... args);

		bool AddCObjectMember(const string& func_name, void* object, CFunctionClosureId closure);

		Index table_location_;
		Environment env_;
	};


	template <typename... ArgumentTypes>
	bool InteractableObject::CallLuaFunc(const string& func_name, ArgumentTypes... args) {
		return PrepareLuaFunc(func_name) && env_.StoreToStack(Index(1)) && env_.CallFunctionWithArgsOnStack<1>(args...);
	}

}  // namespace Lua