#pragma once

namespace Lua {

	class Index
	{
	public:
		explicit Index(int stack_index);
		~Index();

		Index Offset(int num_elements_added);

		int index_;
	};

}  // namespace Lua