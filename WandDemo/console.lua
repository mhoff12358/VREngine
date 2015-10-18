testarray = {}
testarray[1] = 5
testarray[2] = 67
testarray[3] = 52

test2array = {}
test2array["a"] = 6
test2array["b"] = 9

function create_actor()
	actor_table = {}
	actor_table.model_file_name = "console.obj"

	actor_table.output_format = {
		["model_modifier"] = {
			["axis_swap"] = { 0, 1, 2 },
			["axis_scale"] = { 1, 1, 1 },
			["invert_texture_axis"] = { false, true }
		}
	}

	actor_table.model_parentage = {
		[""] = {
			{ "terminal_Plane.001", "grate_Plane" }
		},
		["terminal_Plane.001"] = {
			{ "button2_Circle.001" },
			{ "button1_Circle" }
		}
	}

	actor_table.shader_file_name = "texturedspecular.hlsl"
	actor_table.texture_file_name = "console.png"
	
	actor_table.interactable_objects = {
		{
			["interactable_type"] = "Quad",
			["id"] = "terminal",
			["point_1"] = { -0.75, 1.5, 0 },
			["point_2"] = { .75, 1.5, 0 },
			["point_3"] = { -0.75, 2.207107, -0.707107 },
			["parent_component"] = "terminal_Plane.001"
		}, {
			["interactable_type"] = "Circle",
			["id"] = "green_button",
			["radius"] = 0.188,
			["center"] = { 0.3125, 1.9243, -0.2828 },
			["normal"] = { 0, 1, 1 },
			["right"] = { 1, 0, 0 },
			["parent_component"] = "terminal_Plane.001"
		}, {
			["interactable_type"] = "Circle",
			["id"] = "red_button",
			["radius"] = 0.188,
			["center"] = { -0.3125, 1.9243, -0.2828 },
			["normal"] = { 0, 1, 1 },
			["right"] = { 1, 0, 0 },
			["parent_component"] = "terminal_Plane.001"
		}
	}
	return actor_table
end