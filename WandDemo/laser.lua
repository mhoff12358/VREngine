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
	
	actor_table.interaction_callbacks = OwnsPositionables:NEW({
		initialize = function (self)
			self:add_positionable("Cannon")
			scene_table.weapons:add_weapon(self:get_positionable("Cannon"))

			--self:calc_model_matrix()

			self.is_right = false
			
			self.callbacks.add_listener("space_down", self.cpp_interface)
			self.callbacks.add_listener("update_tick", self.cpp_interface)
		end,
		initialize_sided = function (self, right_side)
			x_scale = -1
			if right_side then x_scale = 1 end
			self.is_right = right_side
			self:get_positionable("Cannon"):position(
				{ 2 * x_scale, .75, -0.5 },
				nil,
				{ .375, .375, .5 })
			--self:calc_model_matrix()
			self.projectile = scene_table:add_actor_and_init("projectile.lua", "projectile")
		end,
		calc_model_matrix = function (self)
			self.callbacks.set_component_transformation("Cannon", {
				{
					["matrix_type"] = "scale",
					["scale"] = self.scale,
				}, {
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
				}, {
					["matrix_type"] = "translation",
					["x"] = self.position[1],
					["y"] = self.position[2],
					["z"] = self.position[3],
				}
				})
		end,
		fire = function (self)
			name, dist = self.callbacks.raycast("Cannon")
			location = self:get_positionable("Cannon").location
			offset = quaternion.apply(self:get_positionable("Cannon").orientation, {0, 0, -dist})
			self.projectile:place(
				{
					location[1] + offset[1],
					location[2] + offset[2],
					location[3] + offset[3],
				},
				quaternion.identity(),
				{ dist / 50, dist / 50, dist / 50 }, 
				{0, 0, 0})
		end,
		space_down = function (self)
			if self.is_right then
				print("Creating projectile")
				self:fire()
				print(name..", "..dist)
			end
		end,
		set_new_orientation = function (self, pitch, yaw)
			self.pitch = pitch
			self.yaw = yaw
			--self:calc_model_matrix()
		end,
		update_tick = function (self, tick_duration_ms)
			self:fire()
		end,
	})

	return actor_table
end
