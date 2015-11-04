require "quaternion"

function create_actor(ident)
	actor_table = {}
	actor_table.model_file_name = "funnel.obj"

	actor_table.output_format = {
		["model_modifier"] = {
			["axis_swap"] = { 0, 1, 2 },
			["axis_scale"] = { 1, 1, 1 },
			["invert_texture_axis"] = { true, true }
		},
		["vertex_type"] = "normal"
	}

	actor_table.model_parentage = {
		[""] = { { "Funnel" } },
	}

	actor_table.settings_blocks = {
		{
			["components"] = { "Funnel" },
			["shader_file_name"] = "glass.hlsl",
			["texture_file_name"] = "|back_buffer",
			["entity_group_number"] = 1,
			["shader_settings_format"] = {
				["pipeline_stage"] = "vertex",
				["data_format"] = { { "float", 3 }, { "float", 1 }, { "float", 3 }, { "float", 1 }, { "float", 1 } }
			},
		},
	}

	actor_table.interactable_objects = {
		{
			["interactable_type"] = "Frustum",
			["id"] = ident .. "|funnel",
			["base_radius"] = 5,
			["top_radius"] = 1,
			["base_center"] = { 0, 4.5, 0 },
			["top_center"] = { 0, 1, 0 },
			["right_radius"] = { 1, 0, 0 },
			["parent_component"] = "Funnel"
		}
	}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self.position = { 0.5, 0, -0.75 }
		self.scale = 0.0625
		self.move_factor = 0.125
		self.opening_size_sq = 20.25
		
		self.add_listener("update_tick", self.cpp_interface)
		self.add_listener("wiimote_button", self.cpp_interface)
		
		self.set_component_transformation("Funnel", {
		{
			["matrix_type"] = "scale",
			["scale"] = { self.scale, self.scale, self.scale },
		}, {
			["matrix_type"] = "translation",
			["x"] = self.position[1],
			["y"] = self.position[2],
			["z"] = self.position[3],
		}
		})

		self.set_constant_buffer(0, { { 1, 0, 0 }, { .005 }, { 1, 1, 1 }, { 10 }, { 0.2 } })
	end
	function actor_table.interaction_callbacks.get_opening (self)
		return { 0, 4.5 * self.scale, 0}
	end
	function actor_table.interaction_callbacks.fill (self, drain_location, amount)
		position_difference = { drain_location[1] - self.position[1], drain_location[3] - self.position[3] }
		if position_difference[1] * position_difference[1] + position_difference[2] * position_difference[2] < self.opening_size_sq * self.scale * self.scale then
			for index, bottle in pairs(all_bottles) do
				if (bottle ~= self) then
					bottle:fill(self.position, amount)
				end
			end
		end
	end
	function actor_table.interaction_callbacks.drain (self, amount)
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
			self.set_component_transformation("Funnel", {
			{
				["matrix_type"] = "scale",
				["scale"] = { self.scale, self.scale, self.scale },
			}, {
				["matrix_type"] = "translation",
				["x"] = self.position[1],
				["y"] = self.position[2],
				["z"] = self.position[3],
			}
			})
		end
	end
	function actor_table.interaction_callbacks.wiimote_button_up (self, pressed)
		if pressed[11] then
			self.held = false
			self.remove_listener("wiimote_position", self.cpp_interface)
		end
	end
	function actor_table.interaction_callbacks.update_tick (self, tick_duration_ms)
		self:drain(tick_duration_ms)
	end

	return actor_table
end