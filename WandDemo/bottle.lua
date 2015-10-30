require "quaternion"

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
			{ "Outer" },
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
			["shader_file_name"] = "fluid.hlsl",
			["components"] = { "Inner" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "pixel",
				["data_format"] = { {"float", 4}, {"float", 1} }
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
		self.orientation = quaternion.identity()
		self.fluid_height = 1.0
		self.fluid_color = { 0.2, 0.75, .02, 1 }
		self.add_update_tick_listener(self.actor)
		if (self.set_component_transformation ~= nil) then
			self.set_component_transformation("empty", {
			{
					["matrix_type"] = "scale",
					["scale"] = {0.0625, 0.0625, 0.0625},
			}, {
					["matrix_type"] = "translation",
					["x"] = 0,
					["y"] = 0,
					["z"] = -0.75,
			}})
		end
		self.set_constant_buffer(0, { { 1, 0, 0 }, { .005 }, { 1, 1, 1 }, { 10 }, { 0.2 } })
		self.set_constant_buffer(1, { self.fluid_color, { self.fluid_height } })
	end
	function actor_table.interaction_callbacks.look_entered (self)
		self.add_mouse_movement_listener(self.actor)
	end
	function actor_table.interaction_callbacks.look_left (self)
		self.remove_mouse_movement_listener(self.actor)
	end
	function actor_table.interaction_callbacks.mouse_motion (self, mouse_motion)
		self.orientation = quaternion.mult(quaternion.mult(
			quaternion.rotation_about_axis(quaternion.AID_Z, 0.01 * mouse_motion[1]),
			quaternion.rotation_about_axis(quaternion.AID_X, 0.01 * mouse_motion[2])),
			self.orientation)
		self.set_component_transformation("empty", {
		{
			["matrix_type"] = "scale",
			["scale"] = {0.0625, 0.0625, 0.0625},
		}, {
			["matrix_type"] = "quaternion_rotation",
			["quaternion"] = self.orientation
		}, {
			["matrix_type"] = "translation",
			["x"] = 0,
			["y"] = 0,
			["z"] = -0.75,
		}
		})
	end
	function actor_table.interaction_callbacks.update_tick (self, tick_duration_ms)
		if (math.acos(self.orientation[4]) * 2 > 3.14/2.0) then
			self.fluid_height = math.max(self.fluid_height - 0.001 * tick_duration_ms, 0.0)
			self.set_constant_buffer(1, { self.fluid_color, { self.fluid_height } })
		end
	end

	return actor_table
end