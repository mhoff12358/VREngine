require "cpp_interface"

function create_scene ()
	scene_table = CppInterface:NEW({})

	actor_interfaces = {}
	user_input = {
		aim_movement = { 0, 0 }
	}

	function scene_table.initialize(self)
		self:add_actor_and_init("cockpit.lua", "cockpit")
		self:add_actor_and_init("terrain.lua", "terrain")
		self:add_actor_and_init("glowy_console.lua", "glowy_console")
		self:add_actor_and_init("skybox.lua", "skybox")
		self.weapons = self:add_actor_and_init("weapons.lua", "weapons")
		
		self.weapons:add_reticle(self:add_actor_and_init("reticle.lua", "reticle"))
		right_laser = self:add_actor_and_init("laser.lua", "right_laser")
		right_laser:initialize_sided(true)
		--weapons:add_weapon(right_laser)
		
		-- The targets are currently drawn after the reticle, and since there is no sorting on the
		-- rendering side this means that the bounding box of the reticle sees through the
		-- targets. This could be fixed by putting them before it in initialization order, but a
		-- more long term fix is preferable.
		target1 = self:add_actor_and_init("target.lua", "target1")
		target1.position = { 0, 0, -50 }
		target1.radius = 5
		target1:calc_model_matrix()
		
		target2 = self:add_actor_and_init("target.lua", "target1")
		target2.position = { 10, 30, -80 }
		target2.radius = 10
		target2:calc_model_matrix()
		
		target3 = self:add_actor_and_init("target.lua", "target1")
		target3.position = { 0, 20, -40 }
		target3.radius = 5
		target3:calc_model_matrix()
	end

	function scene_table.add_actor_and_init(self, script_name, actor_name)
		interface = self.callbacks.add_actor(script_name, actor_name)
		interface:initialize()
		table.insert(actor_interfaces, interface)
		return interface
	end

	return scene_table
end