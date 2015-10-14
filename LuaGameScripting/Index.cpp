#include "stdafx.h"
#include "Index.h"

namespace Lua {

	Index::Index(int stack_index) : index_(stack_index)
	{
	}


	Index::~Index()
	{
	}

	Index Index::Offset(int num_elements_added) {
		if (index_ < 0) {
			return Index(min(index_ - num_elements_added, 0));
		}
		return Index(index_);
	}

}  // namespace Lua