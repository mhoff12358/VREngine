require "quaternion"

function create_actor(ident)
	actor_table = {}
	
	actor_table.model_definition = {
		["model_file_name"] = "projectile.obj",
		["output_format"] = {
			["model_modifier"] = {
				["axis_swap"] = { 0, 1, 2 },
				["axis_scale"] = { 1, 1, 1 },
				["invert_texture_axis"] = { false, true }
			},
		}
	}

	actor_table.model_parentage = {
		[""] = { { "Projectile" } },
	}

	actor_table.settings_blocks = {
		{
			["shader_file_name"] = "solidcolor.hlsl",
			["components"] = { "Projectile" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "pixel",
				["data_format"] = { { "float", 4 } }
			},
		}
	}

	actor_table.interactable_objects = {}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self.position = { 0, 0, 0 }
		self.scale = { 1, 1, 1 }
		self.orientation = quaternion.identity()
		self.speed = 0
		self:calc_model_matrix()

		self.add_listener("update_tick", self.cpp_interface)
		self.set_constant_buffer(0, { { 0, 0, 1, 1 } })
	end
	function actor_table.interaction_callbacks.place (self, position, orientation, scale, speed)
		self.position = position
		self.orientation = orientation
		self.speed = speed
		self.scale = { scale, scale, scale }
		self:calc_model_matrix()
	end
	function actor_table.interaction_callbacks.calc_model_matrix (self)
		self.set_component_transformation("Projectile", {
			{
				["matrix_type"] = "scale",
				["scale"] = self.scale,
			}, {
				["matrix_type"] = "quaternion_rotation",
				["quaternion"] = self.orientation,
			}, {
				["matrix_type"] = "translation",
				["x"] = self.position[1],
				["y"] = self.position[2],
				["z"] = self.position[3],
			}
			})
	end
	function actor_table.interaction_callbacks.update_tick (self, tick_duration_ms)
	end

	return actor_table
end