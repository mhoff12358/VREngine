require "quaternion"

function create_actor(ident)
	actor_table = {}
	
	actor_table.model_definition = {
		["model_file_name"] = "laser.obj",
		["output_format"] = {
			["model_modifier"] = {
				["axis_swap"] = { 0, 1, 2 },
				["axis_scale"] = { 1, 1, 1 },
				["invert_texture_axis"] = { false, true }
			},
		}
	}

	actor_table.model_parentage = {
		[""] = { { "Cannon" } },
	}

	actor_table.settings_blocks = {
		{
			["shader_file_name"] = "textured.hlsl",
			["components"] = { "Cannon" },
			["texture_file_name"] = "cannon.png",
		},
	}

	actor_table.interactable_objects = { }
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self.position = { 0, 0, 0 }
		self.scale = { 1, 1, 1 }
		self.orientation = quaternion.identity()
		self:calc_model_matrix()
	end
	function actor_table.interaction_callbacks.initialize_sided (self, right_side)
		x_scale = -1
		if right_side then x_scale = 1 end
		self.position = { 2 * x_scale, .75, -0.5 }
		self.scale = { .375, .375, .5 }
		self:calc_model_matrix()
	end
	function actor_table.interaction_callbacks.calc_model_matrix (self)
		self.set_component_transformation("Cannon", {
			{
				["matrix_type"] = "scale",
				["scale"] = self.scale,
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

	return actor_table
end