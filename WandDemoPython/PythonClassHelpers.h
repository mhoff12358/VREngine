#pragma once
#include "stdafx.h"

#include "TemplateJank.h"

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


template <typename Container, typename key_type>
	auto contains_temp(Container& container, key_type const& key, special_) -> decltype(key == key, bool()) {
		return std::find(container.begin(), container.end(), key)
			!= container.end();
	}

template <typename Container, typename key_type>
	auto contains_temp(Container& container, key_type const& key, less_special_) -> bool {
		return false;
	}

template <
	class Container>
	class DerivedVec : public vector_indexing_suite<Container, false, DerivedVec<Container>> {
	public:
		static bool contains(Container& container, key_type const& key) {
			return contains_temp<Container, key_type>(container, key, special_());
		}

};

template <typename IndexType, typename ValueType, typename PyType>
auto& CreateVectorIndexing(boost::python::class_<PyType>& c) {
	//return c
	//	.def("__getitem__", static_cast<ValueType*(*)(PyType&, IndexType)>([](PyType& t, IndexType i)->ValueType*{ return &t[i]; }), return_value_policy<reference_existing_object>())
	//	.def("__setitem__", static_cast<void(*)(PyType&, IndexType, ValueType)>([](PyType& t, IndexType i, ValueType v)->void {t[i] = v;}));
	return c
		.def(DerivedVec<PyType>());
}

namespace boost {
	namespace python {
		template <class Container, bool NoProxy, class DerivedPolicies>
		class array_indexing_suite;

		namespace detail
		{
			template <class Container, bool NoProxy>
			class final_array_derived_policies
				: public array_indexing_suite<Container,
				NoProxy, final_array_derived_policies<Container, NoProxy> > {};
		}

		template <
			class Container,
			bool NoProxy = false,
			class DerivedPolicies
			= detail::final_array_derived_policies<Container, NoProxy> >
			class array_indexing_suite
			: public indexing_suite<Container, DerivedPolicies, NoProxy>
		{
		public:

			typedef typename Container::value_type data_type;
			typedef typename Container::value_type key_type;
			typedef typename Container::size_type index_type;
			typedef typename Container::size_type size_type;
			typedef typename Container::difference_type difference_type;

			static
				typename mpl::if_<
				is_class<data_type>
				, data_type&
				, data_type
				>::type
				get_item(Container& container, index_type i)
			{
				return container[i];
			}

			static object
				get_slice(Container& container, index_type from, index_type to)
			{
				if (from > to)
					return object(Container());
				return object(vector<data_type>(container.begin() + from, container.begin() + to));
			}

			static void
				set_item(Container& container, index_type i, data_type const& v)
			{
				container[i] = v;
			}

			static size_t
				size(Container& container)
			{
				return container.size();
			}

			static bool
				contains(Container& container, key_type const& key)
			{
				return contains_temp<Container, key_type>(container, key, special_());
			}

			static index_type
				get_min_index(Container& /*container*/)
			{
				return 0;
			}

			static index_type
				get_max_index(Container& container)
			{
				return container.size();
			}

			static bool
				compare_index(Container& /*container*/, index_type a, index_type b)
			{
				return a < b;
			}

			static index_type
				convert_index(Container& container, PyObject* i_)
			{
				extract<long> i(i_);
				if (i.check())
				{
					long index = i();
					if (index < 0)
						index += DerivedPolicies::size(container);
					if (index >= long(container.size()) || index < 0)
					{
						PyErr_SetString(PyExc_IndexError, "Index out of range");
						throw_error_already_set();
					}
					return index;
				}

				PyErr_SetString(PyExc_TypeError, "Invalid index type");
				throw_error_already_set();
				return index_type();
			}

        static void 
        delete_item(Container& container, index_type i)
        { 
        }
        
        static void 
        delete_slice(Container& container, index_type from, index_type to)
        { 
        }

        static void 
        set_slice(Container& container, index_type from, 
            index_type to, data_type const& v)
        { 
            if (from > to) {
                return;
            }
            else {
				for (auto iter = container.begin() + from; iter != container.begin() + to; ++iter) {
					*iter = v;
				}
            }
        }

        template <class Iter>
        static void 
        set_slice(Container& container, index_type from, 
            index_type to, Iter first, Iter last)
        { 
            if (from > to) {
				return;
            }
            else {
				for (auto iter = container.begin() + from;
					(iter != container.begin() + to) && (first != last); ++iter) {
					*iter = *first;
					++first;
				}
            }
        }

		};
	}
}

template <typename IndexType, typename ValueType, typename PyType>
auto& CreateArrayIndexing(boost::python::class_<PyType>& c) {
	//return c
	//	.def("__getitem__", static_cast<ValueType*(*)(PyType&, IndexType)>([](PyType& t, IndexType i)->ValueType*{ return &t[i]; }), return_value_policy<reference_existing_object>())
	//	.def("__setitem__", static_cast<void(*)(PyType&, IndexType, ValueType)>([](PyType& t, IndexType i, ValueType v)->void {t[i] = v;}));
	return c
		.def(boost::python::array_indexing_suite<PyType>());
}
