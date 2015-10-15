testarray = {}
testarray[1] = 5
testarray[2] = 67
testarray[3] = 52

test2array = {}
test2array["a"] = 6
test2array["b"] = 9

function create_actor()
	interface_table = {}
	interface_table.model_file_name = "console.obj"

	interface_table.model_parentage = {
		[""] = {
			{"terminal_Plane.001", "grate_Plane"}
		},
		["terminal_Plane.001"] = {
			{"button2_Circle.001"},
			{"button1_Circle"}
		}
	}
	return interface_table
end