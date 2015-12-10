require "quaternion"

function create_actor(ident)
	points_per_end = 8

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
			["cylinder"] = {}
		}
	}

	for i=0,2*points_per_end,1 do
		table.insert(actor_table.model_definition.model_vertices.cylinder, {0, 0, 0})
	end

	actor_table.model_parentage = {
		[""] = { { "cylinder" } },
	}

	actor_table.settings_blocks = {
		{
			["components"] = { "cylinder" },
			["shader_file_name"] = "solidcolorforceblue.hlsl",
		},
	}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self.position = { 0, 0, -0.5 }
		self.scale = 0.25

		self.set_component_transformation("square", {
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

		self:make_flow()
	end
	function actor_table.interaction_callbacks.make_flow (self)
		self.set_vertices({
			[0] = {
				{ -1, -1, 0 },
				{ 1, 1, 0 },
				{ 1, -1, 0 },
			}
		})
	end

	return actor_table
end