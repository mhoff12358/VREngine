require "quaternion"

function create_actor(ident)
	actor_table = {}
	
	actor_table.model_definition = {
		["model_file_name"] = "projectile.obj",
		["output_format"] = {
			["model_modifier"] = {
				["axis_swap"] = { 0, 1, 2 },
				["axis_scale"] = { 1, 1, 1 },
				["invert_texture_axis"] = { false, true }
			},
		}
	}

	actor_table.model_parentage = {
		[""] = { { "Projectile" } },
	}

	actor_table.settings_blocks = {
		{
			["shader_file_name"] = "solidcolor.hlsl",
			["components"] = { "Projectile" },
			["shader_settings_format"] = {
				["pipeline_stage"] = "pixel",
				["data_format"] = { { "float", 4 } }
			},
		}
	}

	actor_table.interactable_objects = {}
	
	actor_table.interaction_callbacks = OwnsPositionables:NEW({
		initialize = function (self)
			self:add_positionable("Projectile")
			self.speed = 0

			self.callbacks.add_listener("update_tick", self.cpp_interface)
			self.callbacks.set_constant_buffer(0, { { 0, 0, 1, 1 } })
		end,
		place = function (self, location, orientation, scale, speed)
			self.speed = speed
			self:get_positionable("Projectile"):position(
				location,
				orientation,
				scale)
		end,
		update_tick = function (self, tick_duration_ms)
		end,
	})

	return actor_table
end
