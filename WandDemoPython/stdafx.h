#pragma once

#include "boost/python.hpp"
#include "boost/python/suite/indexing/vector_indexing_suite.hpp"

using boost::python::object;
using boost::python::class_;
using boost::python::return_value_policy;
using boost::python::init;
using boost::python::no_init;
using boost::python::reference_existing_object;
using boost::python::manage_new_object;
using boost::python::bases;
using boost::python::enum_;
using boost::python::dict;
using boost::python::raw_function;
using boost::python::make_function;
using boost::python::error_already_set;
using boost::python::extract;

using boost::python::vector_indexing_suite;

#include "BoostPointerMagic.h"

#include "VRBackend/stl.h"
