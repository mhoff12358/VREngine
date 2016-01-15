require "quaternion"

function create_actor(ident)
	actor_table = {}
	
	actor_table.model_definition = {
		["model_file_name"] = "glowy_console.obj",
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
			["shader_file_name"] = "textured_bloom.hlsl",
			["texture_file_name"] = "glowy_console.png",
			["entity_group_name"] = "bloom",
		},
	}

	actor_table.interactable_objects = {}
	
	actor_table.interaction_callbacks = CppInterface:NEW({
		initialize = function (self)
			self:calc_model_matrix()
		end,
		calc_model_matrix = function (self)
			self.callbacks.set_component_transformation("Plane", {
			{
				["matrix_type"] = "axis_rotation",
				["x"] = 1,
				["y"] = 0,
				["z"] = 0,
				["rotation"] = 3.14/2,
			}, {
				["matrix_type"] = "translation",
				["x"] = 0,
				["y"] = 0,
				["z"] = -3,
			}})
		end,
	})

	return actor_table
end