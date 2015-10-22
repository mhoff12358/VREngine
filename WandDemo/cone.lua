function create_actor()
	actor_table = {}
	actor_table.model_file_name = "cone.obj"

	actor_table.output_format = {
		["model_modifier"] = {
			["axis_swap"] = { 0, 1, 2 },
			["axis_scale"] = { 1, 1, 1 },
			["invert_texture_axis"] = { true, true }
		},
		["vertex_type"] = "normal"
	}

	actor_table.model_parentage = {
		[""] = {
			{ "Cone" }
		}
	}

	actor_table.shader_file_name = "solidcolor.hlsl"
	
	actor_table.interactable_objects = {}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.place_at_console_location (self, new_console_x, new_console_y)
		self.console_x = new_console_x
		self.console_y = new_console_y
		if ((self.clear_component_transformation ~= nil) and (self.apply_to_component_transformation ~= nil)) then
			self.set_component_transformation("Cone", {
			{
					["matrix_type"] = "scale",
					["scale"] = {0.015625, 0.0625, 0.015625}
			},
			{
					["matrix_type"] = "translation",
					["x"] = self.console_x,
					["y"] = 0,
					["z"] = -self.console_y,
			},
			{
					["matrix_type"] = "axis_rotation",
					["x"] = 1,
					["y"] = 0,
					["z"] = 0,
					["rotation"] = 3.1415/4.0
			},
			{
					["matrix_type"] = "translation",
					["x"] = -0.75,
					["y"] = -0.25,
					["z"] = -1,
			}})
		end
	end
	function actor_table.interaction_callbacks.initialize (self)
		self:place_at_console_location(0, 0)
	end

	return actor_table
end