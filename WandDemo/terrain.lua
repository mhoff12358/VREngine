require "quaternion"

function create_actor(ident)
	terrain_resolution = 10

	actor_table = {}
	
	actor_table.model_definition = {
		["output_format"] = {
			["model_modifier"] = {
				["axis_swap"] = { 0, 1, 2 },
				["axis_scale"] = { 1, 1, 1 },
				["invert_texture_axis"] = { true, true }
			},
			["load_as_dynamic"] = true,
			["vertex_type"] = "location",
		},
		["model_vertices"] = {
			["ground"] = {}
		}
	}
	
	for x=-terrain_resolution,terrain_resolution,1 do
		for y=-terrain_resolution,terrain_resolution,1 do
			table.insert(actor_table.model_definition.model_vertices.ground, {x, y, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {x+1, y, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {x+1, y+1, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {x, y, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {x+1, y+1, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {x, y+1, 0})
		end
	end

	actor_table.model_parentage = {
		[""] = { { "ground" } },
	}

	actor_table.settings_blocks = {
		{
			["shader_file_name"] = "terrain.hlsl",
			["texture_file_name"] = "terrain.png",
			["texture_stage_usage"] = { true, true },
			["components"] = { "ground" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "vertex",
				["data_format"] = { { "float", 1 } }
			},
		}
	}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self.set_constant_buffer(0, { { terrain_resolution }, { 5, 1, 0.1 } })
		self.set_component_transformation("ground", {
		{
			["matrix_type"] = "axis_rotation",
			["x"] = 1,
			["y"] = 0,
			["z"] = 0,
			["rotation"] = 3.14/2
		}
		})
	end

	return actor_table
end