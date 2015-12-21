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
			["texture_file_name"] = "target.png",
			["entity_group_number"] = 1,
		},
	}

	actor_table.interactable_objects = {
		{
			["interactable_type"] = "Circle",
			["id"] = "target1",
			["radius"] = 1,
			["center"] = { 0, 0, 0 },
			["normal"] = { 0, 0, 1 },
			["right"] = { 1, 0, 0 },
			["parent_component"] = "Plane"
		}
	}
	
	actor_table.interaction_callbacks = CppInterface:NEW({
		initialize = function (self)
			self.position = { 0, 0, 0 }
			self.orientation = quaternion.identity()
			self.radius = 1

			self:calc_model_matrix()
		end,
		calc_model_matrix = function (self)
			self.callbacks.set_component_transformation("Plane", {
			{
				["matrix_type"] = "scale",
				["scale"] = { self.radius, self.radius, 1 },
			},{
				["matrix_type"] = "quaternion_rotation",
				["quaternion"] = self.orientation,
			},{
				["matrix_type"] = "translation",
				["x"] = self.position[1],
				["y"] = self.position[2],
				["z"] = self.position[3],
			}})
		end,
	})

	return actor_table
end