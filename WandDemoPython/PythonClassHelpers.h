#pragma once
#include "stdafx.h"

template <typename T>
auto CreateClass(string name = "") {
	if (name.empty()) {
		name = typeid(T).name();
	}
	return boost::python::class_<T>(name.c_str(), no_init);
}

template <typename IndexType, typename ValueType, typename PyType>
auto& CreateIndexing(boost::python::class_<PyType>& c) {
	return c
		.def("__getitem__", static_cast<const ValueType(*)(PyType&, IndexType)>([](PyType& t, IndexType i)->const ValueType{ return t[i]; }))
		.def("__setitem__", static_cast<void(*)(PyType&, IndexType, ValueType)>([](PyType& t, IndexType i, ValueType v)->void {t[i] = v;}));
}