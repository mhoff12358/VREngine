#pragma once

#define BOOST_PTR_MAGIC_STRUCT(class_name) \
namespace boost { \
	template <> \
	class_name const volatile * get_pointer<struct class_name const volatile>( \
		struct class_name const volatile *c) \
	{ return c; } \
}

#define BOOST_PTR_MAGIC(class_name) \
namespace boost { \
	template <> \
	class_name const volatile * get_pointer<class class_name const volatile>( \
		class class_name const volatile *c) \
	{ return c; } \
}
