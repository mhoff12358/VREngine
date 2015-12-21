require "quaternion"
require "cpp_interface"

function create_actor(ident)
	actor_table = {}
	
	actor_table.interaction_callbacks = CppInterface:NEW({
		initialize = function (self)
			self.reticles = {}
			self.weapons = {}

			self:set_new_orientation(0, 0)
			self.callbacks.add_listener("update_tick", self.cpp_interface)
		end,
		set_new_orientation = function (self, pitch, yaw)
			self.pitch = pitch
			self.yaw = yaw
			self.quaternion = quaternion.mult(
				quaternion.rotation_about_axis(quaternion.AID_X, pitch),
				quaternion.rotation_about_axis(quaternion.AID_Y, yaw))
			for index, reticle in pairs(self.reticles) do
				reticle:set_new_orientation(self.pitch, self.yaw)
			end
			for index, weapon in pairs(self.weapons) do
				weapon:sparse_position(nil, self.quaternion)
				--weapon:set_new_orientation(self.pitch, self.yaw)
			end
		end,
		update_tick = function (self, tick_duration_ms)
			if ((user_input.aim_movement[1] ~= 0) or (user_input.aim_movement[2] ~= 0)) then
				self:set_new_orientation(
					self.pitch + user_input.aim_movement[1] * tick_duration_ms / 1000,
					self.yaw + user_input.aim_movement[2] * tick_duration_ms / 1000)
			end
		end,
		add_reticle = function (self, reticle)
			table.insert(self.reticles, reticle)
		end,
		add_weapon = function (self, weapon)
			table.insert(self.weapons, weapon)
		end,
	})

	return actor_table
end