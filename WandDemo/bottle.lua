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
			{ "OuterBulb", "OuterTube" },
			{ "InnerBulb", "InnerTube" },
		}
	}

	actor_table.settings_blocks = {
		{
			["shader_file_name"] = "solidcolorforceblue.hlsl",
			["components"] = { "OuterBulb" },
		},
		{
			["shader_file_name"] = "solidcolor.hlsl",
			["components"] = { "InnerBulb" },
		}
	}

	actor_table.preload_shader_file_names = {"solidcolor.hlsl", "solidcolorforceblue.hlsl"}

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
		if (self.set_component_transformation ~= nil) then
			self.set_component_transformation("empty", {
			{
					["matrix_type"] = "scale",
					["scale"] = {0.0625, 0.0625, 0.0625},
			},
			{
					["matrix_type"] = "translation",
					["x"] = 0,
					["y"] = 0,
					["z"] = -0.75,
			}})
		end
		if (self.set_shader ~= nil) then
			--self.set_shader(0, "solidcolorforceblue.hlsl")
		else
			print("Cannot set shader")
		end
	end
	function actor_table.interaction_callbacks.look_entered (self, object_id)
		if (self.set_shader ~= nil) then
			--self.set_shader(0, "solidcolorforceblue.hlsl")
		else
			print("Cannot set shader")
		end
	end
	function actor_table.interaction_callbacks.look_left (self, object_id, new_object_id)
		if (self.set_shader ~= nil) then
			--self.set_shader(0, "solidcolor.hlsl")
		else
			print("Cannot set shader")
		end
	end

	return actor_table
end