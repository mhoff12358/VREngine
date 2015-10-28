function create_actor()
	actor_table = {}
	actor_table.model_file_name = "bottle.obj"

	actor_table.output_format = {
		["model_modifier"] = {
			["axis_swap"] = { 0, 1, 2 },
			["axis_scale"] = { 1, 1, 1 },
			["invert_texture_axis"] = { true, true }
		},
		["vertex_type"] = "normal"
	}

	actor_table.model_parentage = {
		[""] = { { "empty" } },
		["empty"] = {
			{ "Outer", "OuterTube" },
			{ "Inner", "InnerTube" },
		}
	}

	actor_table.settings_blocks = {
		{
			["components"] = { "Outer" },
			["shader_file_name"] = "glass.hlsl",
			["texture_file_name"] = "|back_buffer",
			["entity_group_number"] = 1,
			["shader_settings_format"] = {
				["pipeline_stage"] = "vertex",
				["data_format"] = { { "float", 3 }, { "float", 1 }, { "float", 3 }, { "float", 1 }, { "float", 1 } }
			},
		},
		{
			["shader_file_name"] = "solidcolor.hlsl",
			["components"] = { "Inner" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "pixel",
				["data_format"] = { {"float", 4} }
			},
		}
	}

	actor_table.interactable_objects = {
		{
			["interactable_type"] = "Sphere",
			["id"] = "bottle",
			["radius"] = 0.0625,
			["center"] = { 0, 0, 0 },
			["parent_component"] = ""
		}
	}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		if (self.set_component_transformation ~= nil) then
			self.set_component_transformation("empty", {
			{
					["matrix_type"] = "scale",
					["scale"] = {0.0625, 0.0625, 0.0625},
			},
			{
					["matrix_type"] = "translation",
					["x"] = 0,
					["y"] = 0,
					["z"] = -0.75,
			}})
		end
		self.set_constant_buffer(0, { { 1, 0, 0 }, { .005 }, { 1, 1, 1 }, { 10 }, { 0.2 } })
		self.set_constant_buffer(1, { { 0.2, 0.75, 0.2} })
		if (self.set_shader ~= nil) then
			--self.set_shader(0, "solidcolorforceblue.hlsl")
		else
			print("Cannot set shader")
		end
	end

	return actor_table
end