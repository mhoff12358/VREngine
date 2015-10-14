function create_cpp_interface(interface_name)
	if (interface_name == "") then
		interface_name = "default"
	end
	interface_table = {}
	interface_table.name = interface_name
	function interface_table.test_func(this)
		print "HI!"
	end
	return interface_table
end