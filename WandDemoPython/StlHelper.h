#pragma once
#include "stdafx.h"

#include <type_traits>

struct general_ {};
struct less_special_ : general_ {};
struct special_ : less_special_ {};

template <typename Collection, typename ValueType>
auto ResizeIfPossibleImpl(Collection& c, unsigned int size, special_) -> decltype(c.reserve(size), void()) {
	c.reserve(size);
}

template <typename Collection, typename ValueType>
auto ResizeIfPossibleImpl(Collection& c, unsigned int size, less_special_) -> decltype(c.size(), void()) {
	assert(c.size() == size);
}


template <typename Collection, typename ValueType>
auto ResizeIfPossibleImpl(Collection& c, unsigned int size, general_) -> void {
}

template <typename Collection, typename ValueType>
void ResizeIfPossible(Collection& c, unsigned int size) {
	ResizeIfPossibleImpl<Collection, ValueType>(c, size, special_());
}

template <typename Collection, typename ValueType>
auto PushBackIfPossibleImpl(Collection& c, unsigned int index, ValueType&& value, special_) -> decltype(c.reserve(0), void()) {
	c.push_back(move(value));
}

template <typename Collection, typename ValueType>
auto PushBackIfPossibleImpl(Collection& c, unsigned int index, ValueType&& value, general_) -> void {
	c[index] = value;
}

template <typename Collection, typename ValueType>
void PushBackIfPossible(Collection& c, unsigned int index, ValueType&& value) {
	PushBackIfPossibleImpl<Collection, ValueType>(c, index, move(value), special_());
}

template <typename ValueType, typename Collection>
Collection* CreateFromList(object iterable) {
	unique_ptr<Collection> c = make_unique<Collection>();
	try {
		ssize_t iterable_size = boost::python::len(iterable);
		ResizeIfPossible<Collection, ValueType>(*c, iterable_size);
		for (ssize_t i = 0; i < iterable_size; i++) {
			ValueType v = extract<ValueType>(iterable[i]);
			PushBackIfPossible<Collection, ValueType>(*c, i, move(v));
		}
	}
	catch (error_already_set) {
		PyErr_Print();
	}
	return c.release();
}

template <typename KeyType, typename ValueType, typename Collection>
Collection* CreateFromDict(dict mapping) {
	unique_ptr<Collection> c = make_unique<Collection>();
	try {
		object iterable = mapping.items();
		ssize_t iterable_size = boost::python::len(iterable);
		for (ssize_t i = 0; i < iterable_size; i++) {
			object key_value = iterable[i];
			c->emplace(extract<KeyType>(key_value[0]), extract<ValueType>(key_value[1]));
		}
	}
	catch (error_already_set) {
		PyErr_Print();
	}
	return c.release();
}

/*template <size_t index, typename TupleType>
void CreateTupleFromListImpl(object iterable, TupleType& t) {
}

template <size_t index, typename TupleType, typename FirstType, typename ... Types>
void CreateTupleFromListImpl(object iterable, TupleType& t) {
	std::get<index>(t) = extract<FirstType>(iterable[index]);
	CreateTupleFromListImpl<index + 1, TupleType, Types...>(iterable, t);
}

template <typename ... Types>
tuple<Types...>* CreateTupleFromList(object iterable) {
	assert(boost::python::len(iterable) == sizeof...(Types));
	auto t = make_unique<tuple<Types...>>();
	CreateTupleFromListImpl<0, tuple<Types...>, Types...>(iterable, *t);
	return t.release();
}

object TupleExtract(boost::python::tuple args, dict kwargs) {
	return object();
}*/

template <typename ValueType, typename PyType>
auto& CreateListToCollection(boost::python::class_<PyType> c) {
	return c
		.def("__init__", boost::python::make_constructor(&CreateFromList<ValueType, PyType>))
		.def("Create", &CreateFromList<ValueType, PyType>, return_value_policy<manage_new_object>())
		.staticmethod("Create");
}

template <typename KeyType, typename ValueType, typename PyType>
auto& CreateDictToMap(boost::python::class_<PyType> c) {
	return c
		.def("__init__", boost::python::make_constructor(&CreateFromDict<KeyType, ValueType, PyType>))
		.def("Create", &CreateFromDict<KeyType, ValueType, PyType>, return_value_policy<manage_new_object>())
		.staticmethod("Create");
}

/*template <typename TupleType, size_t N>
struct CreateTupleAccess {
	using tuple_element_t = typename std::tuple_element<N, TupleType>::type;
	static void Create(class_<TupleType> tuple_reference) {
		tuple_reference.add_property((string("a") + std::to_string(N)).c_str(), &AccessElement, &UpdateElement);
		CreateTupleAccess<TupleType, N - 1>::Create(tuple_reference);
	}
	
	static tuple_element_t AccessElement(TupleType& t) {
		return std::get<N>(t);
	}

	static void UpdateElement(TupleType& t, tuple_element_t& value) {
		std::get<N>(t) = value;
	}
};

template <typename TupleType>
struct CreateTupleAccess<TupleType, 0> {
	using tuple_element_t = typename std::tuple_element<0, TupleType>::type;
	static void Create(class_<TupleType> tuple_reference) {
		tuple_reference.add_property((string("a") + std::to_string(0)).c_str(), &AccessElement, &UpdateElement);
	}
	
	static tuple_element_t AccessElement(TupleType& t) {
		return std::get<0>(t);
	}

	static void UpdateElement(TupleType& t, tuple_element_t& value) {
		std::get<0>(t) = value;
	}
};

template<typename ... Types>
void CreateTuple(string names) {
	auto tuple_class = CreateClass<tuple<Types...>>("Tuple" + names);
	CreateTupleAccess<tuple<Types...>, sizeof...(Types) - 1>::Create(tuple_class);
	tuple_class
		.def("__init__", boost::python::make_constructor(&CreateTupleFromList<Types...>));
		//.def("__getitem__", static_cast<const ValueType(*)(PyType&, IndexType)>([](PyType& t, IndexType i)->const ValueType{ return t[i]; }))
		//.def("__setitem__", static_cast<void(*)(PyType&, IndexType, ValueType)>([](PyType& t, IndexType i, ValueType v)->void {t[i] = v;}));
}*/

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

template<typename Key, typename Value>
void CreateMap(string key_name, string value_name) {
	typedef map<Key, Value> MapType;
	CreateDictToMap<Key, Value, MapType>(CreateIndexing<Key, Value, MapType>(CreateClass<MapType>("Map" + key_name + "To" + value_name)));
}

template<typename T, size_t N>
void CreateArrayWithVector(string name) {
	CreateArray<T, N>(name);
	CreateVector<array<T, N>>("Array" + name + std::to_string(N));
}

template <typename T, size_t N>
struct CreateArraysWithVector {
	static void Create(string name) {
		CreateArrayWithVector<T, N>(name);
		CreateArraysWithVector<T, N - 1>::Create(name);
	}
};

template <typename T>
struct CreateArraysWithVector<T, 0> {
	static void Create(string name) {}
};
