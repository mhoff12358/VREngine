require "quaternion"
require "cpp_interface"
require "math"

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
			table.insert(actor_table.model_definition.model_vertices.ground, {x / terrain_resolution, y / terrain_resolution, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {(x+1) / terrain_resolution, y / terrain_resolution, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {(x+1) / terrain_resolution, (y+1) / terrain_resolution, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {x / terrain_resolution, y / terrain_resolution, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {(x+1) / terrain_resolution, (y+1) / terrain_resolution, 0})
			table.insert(actor_table.model_definition.model_vertices.ground, {x / terrain_resolution, (y+1) / terrain_resolution, 0})
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
			["entity_group_name"] = "terrain"
		}
	}
	
	actor_table.interaction_callbacks = CppInterface:NEW({
		initialize = function (self)
			self.plane_scale = 128
			self.height_scales = { 16, 1, 0.25 }
			self.callbacks.set_constant_buffer(0, { { self.plane_scale }, self.height_scales })
			self.callbacks.set_component_transformation("ground", {
			{
				["matrix_type"] = "axis_rotation",
				["x"] = 1,
				["y"] = 0,
				["z"] = 0,
				["rotation"] = 3.14/2
			}, {
				["matrix_type"] = "translation",
				["x"] = 0,
				["y"] = -1.5,
				["z"] = 0,
			}
			})
			
			self.height_data = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data0 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data1 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data2 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data3 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data4 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data5 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data6 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data7 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data8 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data9 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data10 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data11 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data12 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data13 = self.callbacks.lookup_resource(4, "terrain.png", "data")
			self.height_data_size = self.callbacks.lookup_resource(4, "terrain.png", "size")
			print(self.height_data_size[1])
			print(self.height_data_size[2])
			print(self:lookup_grid_height(0, 0))
			print(self:lookup_grid_height(self.height_data_size[1]-1, 0))
			print(self:lookup_grid_height(0, self.height_data_size[2]-1))
			print(self:lookup_grid_height(self.height_data_size[1]-1, self.height_data_size[2]-1))
			
		end,
		lookup_grid_height = function(self, x, y)
			-- Requires x in [0, self.height_data_size[1]-1] and y in x in [0, self.height_data_size[2]-1]
			index = (y * self.height_data_size[1] + x) * 4
			height = 0
			print(x, " ", y, " ", index)
			for color_channel = 1, 3 do
				b = self.height_scales[color_channel]
				a = self.height_data[index + color_channel]
				c = a * b
				height = height + c
			end
			return height
		end,
		lookup_height = function(self, x, y)
			--Normalize the world lookup coordinates to [-1, 1] which map to the texture
			x = x / self.plane_scale
			y = y / self.plane_scale
			--Map [-1, 1] to [0, size-1] so that the coordinates can be used for a texture lookup
			x = (x + 1) * (self.height_data_size[1] / 2.0)
			y = (y + 1) * (self.height_data_size[2] / 2.0)
			min_x = math.floor(x)
			max_x = math.ceil(x)
			min_y = math.floor(y)
			max_y = math.ceil(y)
			if (min_x == max_x) then
				min_y_value = self:lookup_grid_height(min_x, min_y)
				max_y_value = self:lookup_grid_height(min_x, max_y)
			else
				max_proportion = x - min_x
				min_proportion = 1 - max_proportion
				min_y_value = max_proportion * self:lookup_grid_height(max_x, min_y) + min_proportion * self:lookup_grid_height(min_x, min_y)
				max_y_value = max_proportion * self:lookup_grid_height(max_x, max_y) + min_proportion * self:lookup_grid_height(min_x, max_y)
			end
			if (min_y == max_y) then
				return min_y_value
			else
				max_proportion = y - min_y
				min_proportion = 1 - max_proportion
				return max_proportion * max_y_value + min_proportion * min_y_value
			end
		end,
	})

	return actor_table
end