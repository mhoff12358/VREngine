#pragma once
#include "stdafx.h"

template <typename T>
auto CreateClass(string name = "") {
	if (name.empty()) {
		name = typeid(T).name();
	}
	return boost::python::class_<T>(name.c_str(), no_init);
}