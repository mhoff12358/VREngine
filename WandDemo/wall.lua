function create_actor()
	actor_table = {}

	actor_table.model_definition = {
		["model_file_name"] = "tower_inside.obj",
		["output_format"] = {
			["model_modifier"] = {
				["axis_swap"] = { 0, 1, 2 },
				["axis_scale"] = { 1, 1, 1 },
				["invert_texture_axis"] = { false, true }
			}
		}
	}

	actor_table.model_parentage = {
		[""] = {
			{ "empty" }
		},
		["empty"] = {
			{ "Grout" },
			{ "Cobble" }
		}
	}

	actor_table.settings_blocks = {
		{
			["shader_file_name"] = "solidcolor.hlsl",
			["components"] = { "Grout" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "pixel",
				["data_format"] = { { "float", 4 } }
			},
		},
		{
			["shader_file_name"] = "textured.hlsl",
			["texture_file_name"] = "walls.png",
			["components"] = { "Cobble" },
		}
	}
	
	actor_table.interactable_objects = {
	}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self.set_component_transformation("empty", {{
			["matrix_type"] = "translation",
			["x"] = 0,
			["y"] = -1.75,
			["z"] = -1
		}, {
			["matrix_type"] = "scale",
			["scale"] = { 0.5, 0.5, 0.5 },
		}})
		self.set_constant_buffer(0, { { 0.05, 0.05, 0.05, 1 } })
	end

	return actor_table
end