require "quaternion"

function create_actor(ident)
	actor_table = {}
	
	actor_table.model_definition = {
		["model_file_name"] = "reticle.obj",
		["output_format"] = {
			["model_modifier"] = {
				["axis_swap"] = { 0, 1, 2 },
				["axis_scale"] = { 1, 1, 1 },
				["invert_texture_axis"] = { true, true }
			},
		}
	}

	actor_table.model_parentage = {
		[""] = { { "Plane" } },
	}

	actor_table.settings_blocks = {
		{
			["components"] = { "Plane" },
			["shader_file_name"] = "textured.hlsl",
			["texture_file_name"] = "reticle.png",
			["entity_group_number"] = 1,
		},
	}

	actor_table.interactable_objects = {}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self:set_new_orientation(0, 0)
	end
	function actor_table.interaction_callbacks.set_new_orientation (self, pitch, yaw)
		self.pitch = pitch
		self.yaw = yaw
		self:calc_model_matrix()
	end
	function actor_table.interaction_callbacks.calc_model_matrix (self)
		self.set_component_transformation("Plane", {
        {
			["matrix_type"] = "translation",
			["x"] = 0,
			["y"] = 0,
			["z"] = -20,
		},{
			["matrix_type"] = "axis_rotation",
			["x"] = 1,
			["y"] = 0,
			["z"] = 0,
			["rotation"] = self.pitch,
		}, {
			["matrix_type"] = "axis_rotation",
			["x"] = 0,
			["y"] = 1,
			["z"] = 0,
			["rotation"] = self.yaw,
		}})
	end

	return actor_table
end