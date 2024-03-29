require "owns_positionables"

function create_actor()
	actor_table = {}

	actor_table.model_definition = {
		["multi_model"] = {
			{
				["model_file_name"] = "cockpit_bars.obj",
				["load_model_as_dynamic"] = false,
				["output_format"] = {
					["model_modifier"] = {
						["axis_swap"] = { 0, 1, 2 },
						["axis_scale"] = { 1, 1, 1 },
						["invert_texture_axis"] = { false, true }
					},
					["load_as_dynamic"] = false
				},
			}, {
				["model_file_name"] = "laser.obj",
				["output_format"] = {
					["model_modifier"] = {
						["axis_swap"] = { 0, 1, 2 },
						["axis_scale"] = { 1, 1, 1 },
						["invert_texture_axis"] = { false, true }
					},
				},
			},
		},
	}

	actor_table.model_parentage = {
		[""] = {
			{ "MetalBars" },
			{ "LightBulb" },
			{ "Seat" },
			{ "Cannon" },
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
			["entity_group_name"] = "basic"
		},
		{
			["shader_file_name"] = "texturedspecularlightsource.hlsl",
			["texture_file_name"] = "seat.png",
			["components"] = { "Seat" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "vertex",
				["data_format"] = { { "float", 3 }, { "float", 1 } }
			},
			["entity_group_name"] = "basic"
		},
		{
			["shader_file_name"] = "solidcolor.hlsl",
			["components"] = { "LightBulb" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "pixel",
				["data_format"] = { { "float", 4 } }
			},
			["entity_group_name"] = "basic"
		},
		{
			["shader_file_name"] = "textured.hlsl",
			["components"] = { "Cannon" },
			["texture_file_name"] = "cannon.png",
			["entity_group_name"] = "basic"
		}
	}
	
	actor_table.interactable_objects = {}
	
	actor_table.interaction_callbacks = OwnsPositionables:NEW({
		initialize = function (self)
			self.callbacks.set_constant_buffer(2, { { 1, 1, 1, 1 } })
			
			self:add_positionable("Cannon")
			self:add_positionable("LightBulb")

			self:get_positionable("Cannon"):position({ -5, 0, 0 })

			self:set_light_location({ 0, 0.5, 0 })
		end,
		set_light_location = function (self, new_light_location)
			self.light_location = new_light_location
			self.callbacks.set_constant_buffer(0, { self.light_location, { 0 } })
			self.callbacks.set_constant_buffer(1, { self.light_location, { 0 } })
			self:get_positionable("LightBulb"):position(self.light_location)
		end
	})

	return actor_table
end