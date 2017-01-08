#include "stdafx.h"

#include "HandleError.h"

void HandleError() {
	//PyObject *type_ptr = NULL, *value_ptr = NULL, *traceback_ptr = NULL;
	//PyErr_Fetch(&type_ptr, &value_ptr, &traceback_ptr);
	PyErr_Print();
	try {
		//boost::python::handle<> h_tb(traceback_ptr);
		object sys(boost::python::import("sys"));
		object pdb(boost::python::import("pdb"));
		object pdb_fn = pdb.attr("post_mortem");
		object h_tb = sys.attr("last_traceback");
		pdb_fn(h_tb);
	} catch (error_already_set) {
		PyErr_Print();
	}
}
