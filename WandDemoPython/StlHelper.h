#pragma once
#include "stdafx.h"

#include <type_traits>

struct A {
	A() {}
};

template <unsigned int b>
struct B {
	B(A a) {}
};

template <typename Collection>
auto ResizeIfPossibleImpl(Collection& c, unsigned int size, B<0> b) -> decltype(c.resize(size), void()) {
	c.resize(size)
}

template <typename Collection>
auto ResizeIfPossibleImpl(Collection& c, unsigned int size, B<1> b) -> void {

}

template <typename Collection>
auto ResizeIfPossible(Collection& c, unsigned int size) -> void {
	ResizeIfPossibleImpl(c, size, A());
}

template <typename ValueType, typename Collection>
Collection* CreateFromList(object iterable) {
	unique_ptr<Collection> c = make_unique<Collection>();
	try {
		unsigned int iterable_size = boost::python::len(iterable);
		ResizeIfPossible(*c, iterable_size);
		for (unsigned int i = 0; i < iterable_size; i++) {
			(*c)[i] = extract<ValueType>(iterable[i]);
		}
	}
	catch (error_already_set) {
		PyErr_Print();
	}
	return c.release();
}

template <typename ValueType, typename PyType>
auto& CreateListToCollection(boost::python::class_<PyType> c) {
	return c
		.def("__init__", boost::python::make_constructor(&CreateFromList<ValueType, PyType>))
		.def("Create", &CreateFromList<ValueType, PyType>, return_value_policy<manage_new_object>())
		.staticmethod("Create");
}

template<typename T, size_t N>
void CreateArray(string name) {
	CreateListToCollection<T, array<T, N>>(CreateIndexing<size_t, T, array<T, N>>(CreateClass<array<T, N>>("Array" + name + std::to_string(N))));
}

template<typename T, size_t N>
void CreateArrays(string name) {
	if (N > 0) {
		CreateArray<T, N>(name);
		CreateArrays<T, N - 1>(name);
	}
}

template<typename T>
void CreateVector(string name) {
	CreateListToCollection<T, vector<T>>(CreateIndexing<size_t, T, vector<T>>(CreateClass<vector<T>>("Vector" + name)));
}

template<typename T, size_t N>
void CreateArrayWithVector(string name) {
	CreateArray<T, N>(name);
	CreateVector<array<T, N>>("Array" + name + std::to_string(N));
}

template<typename T, size_t N, bool recurse = true>
void CreateArraysWithVector(string name);

template<typename T, size_t N, bool recurse>
std::enable_if<recurse> CreateArraysWithVector(string name) {
	CreateArrayWithVector<T, N>(name);
	CreateArraysWithVector<T, N - 1, N > 0>(name);
}

template<typename T, size_t N, bool recurse>
void CreateArraysWithVector(string name) {}