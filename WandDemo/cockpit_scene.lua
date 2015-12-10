function create_scene ()
	scene_table = {}

	actor_interfaces = {}
	all_bottles = {}

	function scene_table.initialize(self)
		self:add_actor_and_init("cockpit.lua", "cockpit")
		self:add_actor_and_init("terrain.lua", "terrain")
	end

	function scene_table.add_actor_and_init(self, script_name, actor_name)
		interface = self.add_actor(script_name, actor_name)
		interface:initialize()
		table.insert(actor_interfaces, interface)
		return interface
	end

	return scene_table
end