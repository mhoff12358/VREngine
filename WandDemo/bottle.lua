require "quaternion"

function create_actor(ident)
	actor_table = {}
	
	actor_table.model_definition = {
		["model_file_name"] = "bottle.obj",
		["output_format"] = {
			["model_modifier"] = {
				["axis_swap"] = { 0, 1, 2 },
				["axis_scale"] = { 1, 1, 1 },
				["invert_texture_axis"] = { true, true }
			},
			["vertex_type"] = "normal"
		}
	}

	actor_table.model_parentage = {
		[""] = { { "empty" } },
		["empty"] = {
			{ "Outer" },
			{ "Inner" },
		}
	}

	actor_table.settings_blocks = {
		{
			["components"] = { "Outer" },
			["shader_file_name"] = "glass.hlsl",
			["texture_file_name"] = "|first_stage_buffer",
			["entity_group_number"] = 1,
			["shader_settings_format"] = {
				["pipeline_stage"] = "vertex",
				["data_format"] = { { "float", 3 }, { "float", 1 }, { "float", 3 }, { "float", 1 }, { "float", 1 } }
			},
		},
		{
			["components"] = { "Inner" },
			["shader_file_name"] = "fluid.hlsl",
			["shader_settings_format"] = {
				["pipeline_stage"] = "pixel",
				["data_format"] = { {"float", 4}, {"float", 1} }
			},
		},
	}

	actor_table.interactable_objects = {
		{
			["interactable_type"] = "Sphere",
			["id"] = ident .. "|bottle",
			["radius"] = 0.0625,
			["center"] = { 0, 0, 0 },
			["parent_component"] = ""
		}
	}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self.fluid_height = 2.5
		self.fluid_color = { math.random(), math.random(), math.random(), 1 }

		self.position = { 0, 0, -0.75 }
		self.scale = 0.0625
		self.move_factor = 0.125

		self.looked_at = false
		self.held = false

		self.opening_size_sq = 0.1464439824
		
		self.add_listener("update_tick", self.cpp_interface)
		self.add_listener("wiimote_button", self.cpp_interface)
		
		self.set_orientation(self, quaternion.identity())

		self.set_constant_buffer(0, { { 1, 0, 0 }, { .005 }, { 1, 1, 1 }, { 10 }, { 0.2 } })
		self.set_constant_buffer(1, { self.fluid_color, { self.fluid_height } })
	end
	function actor_table.interaction_callbacks.get_opening (self)
		return quaternion.apply(self.orientation, { 0, 2.5 * self.scale, 0})
	end
	function actor_table.interaction_callbacks.set_orientation (self, new_orientation)
		self.orientation = new_orientation
		self.opening_location = quaternion.apply(self.orientation, { 0, 2.5 * self.scale, 0 })

		self.set_component_transformation("empty", {
		{
			["matrix_type"] = "scale",
			["scale"] = { self.scale, self.scale, self.scale },
		}, {
			["matrix_type"] = "quaternion_rotation",
			["quaternion"] = self.orientation
		}, {
			["matrix_type"] = "translation",
			["x"] = self.position[1],
			["y"] = self.position[2],
			["z"] = self.position[3],
		}
		})
	end
	function actor_table.interaction_callbacks.fill (self, drain_location, amount)
		position_difference = { drain_location[1] - self.position[1], drain_location[3] - self.position[3] }
		if position_difference[1] * position_difference[1] + position_difference[2] * position_difference[2] < self.opening_size_sq * self.scale * self.scale then
			self.fluid_height = math.min(self.fluid_height + 0.001 * amount, 2.5)
			self.set_constant_buffer(1, { self.fluid_color, { self.fluid_height } })
		end
	end
	function actor_table.interaction_callbacks.drain (self, amount)
		if (math.acos(self.orientation[4]) > 3.14/4.0) then
			self.fluid_height = math.max(self.fluid_height - 0.001 * amount, 0.0)
			self.set_constant_buffer(1, { self.fluid_color, { self.fluid_height } })

			for index, bottle in pairs(all_bottles) do
				if (bottle ~= self) then
					bottle:fill({ self.opening_location[1] + self.position[1], self.opening_location[2] + self.position[2], self.opening_location[3] + self.position[3] }, amount)
				end
			end
		end
	end
	function actor_table.interaction_callbacks.look_entered (self)
		self.looked_at = true
	end
	function actor_table.interaction_callbacks.look_left (self)
		self.looked_at = false
	end
	function actor_table.interaction_callbacks.wiimote_button_down (self, pressed)
		if pressed[11] and self.looked_at then
			self.held = true
			self.add_listener("wiimote_position", self.cpp_interface)
		end
		if self.held then
			if pressed[1] then
				self.position[1] = self.position[1] - self.move_factor
			end
			if pressed[2] then
				self.position[1] = self.position[1] + self.move_factor
			end
			if pressed[3] then
				self.position[2] = self.position[2] - self.move_factor
			end
			if pressed[4] then
				self.position[2] = self.position[2] + self.move_factor
			end
		end
	end
	function actor_table.interaction_callbacks.wiimote_button_up (self, pressed)
		if pressed[11] then
			self.held = false
			self.remove_listener("wiimote_position", self.cpp_interface)
			self:set_orientation( { 0, 0, 0, 1 } )
		end
	end
	function actor_table.interaction_callbacks.wiimote_position (self, orientation)
		self.set_orientation(self, orientation)
	end
	function actor_table.interaction_callbacks.update_tick (self, tick_duration_ms)
		self:drain(tick_duration_ms)
	end

	return actor_table
end