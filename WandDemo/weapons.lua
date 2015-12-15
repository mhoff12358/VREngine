require "quaternion"

function create_actor(ident)
	actor_table = {}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
		self.reticles = {}
		self.weapons = {}

		self:set_new_orientation(0, 0)
		self.add_listener("update_tick", self.cpp_interface)
	end
	function actor_table.interaction_callbacks.set_new_orientation (self, pitch, yaw)
		self.pitch = pitch
		self.yaw = yaw
		for index, reticle in pairs(self.reticles) do
			reticle:set_new_orientation(self.pitch, self.yaw)
		end
		for index, weapon in pairs(self.weapons) do
			weapon:set_new_orientation(self.pitch, self.yaw)
		end
	end
	function actor_table.interaction_callbacks.update_tick (self, tick_duration_ms)
		self:set_new_orientation(
			self.pitch + user_input.aim_movement[1] * tick_duration_ms / 1000,
			self.yaw + user_input.aim_movement[2] * tick_duration_ms / 1000)
	end
	function actor_table.interaction_callbacks.add_reticle (self, reticle)
		table.insert(self.reticles, reticle)
	end
	function actor_table.interaction_callbacks.add_weapon (self, weapon)
		table.insert(self.weapons, weapon)
	end

	return actor_table
end