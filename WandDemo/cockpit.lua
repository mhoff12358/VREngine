function create_actor()
	actor_table = {}

	actor_table.model_definition = {
		["model_file_name"] = "cockpit_bars.obj",
		["load_model_as_dynamic"] = false,
		["output_format"] = {
			["model_modifier"] = {
				["axis_swap"] = { 0, 1, 2 },
				["axis_scale"] = { 1, 1, 1 },
				["invert_texture_axis"] = { false, true }
			},
			["load_as_dynamic"] = false
		}
	}

	actor_table.model_parentage = {
		[""] = {
			{ "MetalBars" },
			{ "LightBulb" },
			{ "Seat" },
		}
	}

	actor_table.settings_blocks = {
		{
			["shader_file_name"] = "texturedspecularlightsource.hlsl",
			["texture_file_name"] = "metal_bars.png",
			["components"] = { "MetalBars" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "vertex",
				["data_format"] = { { "float", 3 }, { "float", 1 } }
			},
		},
		{
			["shader_file_name"] = "texturedspecularlightsource.hlsl",
			["texture_file_name"] = "seat.png",
			["components"] = { "Seat" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "vertex",
				["data_format"] = { { "float", 3 }, { "float", 1 } }
			},
		},
		{
			["shader_file_name"] = "solidcolor.hlsl",
			["components"] = { "LightBulb" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "pixel",
				["data_format"] = { { "float", 4 } }
			},
		}
	}
	
	actor_table.interactable_objects = {}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self:set_light_location({ 0, 0.5, 0 })
		self.set_constant_buffer(2, { { 1, 1, 1, 1 } })
	end
	function actor_table.interaction_callbacks.set_light_location(self, new_light_location)
		self.light_location = new_light_location
		self.set_constant_buffer(0, { self.light_location, { 0 } })
		self.set_constant_buffer(1, { self.light_location, { 0 } })
		self.set_component_transformation("LightBulb", {
		{
			["matrix_type"] = "translation",
			["x"] = self.light_location[1],
			["y"] = self.light_location[2],
			["z"] = self.light_location[3],
		}
		})
	end

	return actor_table
end