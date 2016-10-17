#pragma once
#include "stdafx.h"

template <typename ValueType, typename Collection>
Collection* CreateFromList(object iterable) {
	unique_ptr<Collection> c = make_unique<Collection>();
	try {
		unsigned int iterable_size = boost::python::len(iterable);
		for (unsigned int i = 0; i < iterable_size; i++) {
			(*c)[i] = extract<ValueType>(iterable[i]);
		}
	}
	catch (error_already_set) {
		PyErr_Print();
	}
	return c.release();
}

template <typename ValueType, typename Collection>
Collection* CreateFromListWithResize(object iterable) {
	unique_ptr<Collection> c = make_unique<Collection>();
	try {
		unsigned int iterable_size = boost::python::len(iterable);
		//c->resize(iterable_size);
		for (unsigned int i = 0; i < iterable_size; i++) {
			c->push_back(extract<ValueType>(iterable[i]));
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

template <typename ValueType, typename PyType>
auto& CreateListToCollectionWithResize(boost::python::class_<PyType> c) {
	return c
		.def("__init__", boost::python::make_constructor(&CreateFromListWithResize<ValueType, PyType>))
		.def("Create", &CreateFromListWithResize<ValueType, PyType>, return_value_policy<manage_new_object>())
		.staticmethod("Create");
}
