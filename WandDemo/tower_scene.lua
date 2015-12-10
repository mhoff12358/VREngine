function create_scene ()
	scene_table = {}

	actor_interfaces = {}
	all_bottles = {}

	function scene_table.initialize(self)
		self.bottle_count = 1
		wall_interface = self:add_actor_and_init("wall.lua", "wall")

		water_flow_interface = self:add_actor_and_init("water_flow.lua", "water_flow")

		funnel_interface = self:add_actor_and_init("funnel.lua", "funnel")
		table.insert(all_bottles, funnel_interface)

		self.add_listener("space_down", self.cpp_interface)
	end

	function scene_table.add_actor_and_init(self, script_name, actor_name)
		interface = self.add_actor(script_name, actor_name)
		interface:initialize()
		table.insert(actor_interfaces, interface)
		return interface
	end

	function scene_table.space_down(self)
		new_interface = self.add_actor("bottle.lua", "bottle" .. tostring(self.bottle_count))
		new_interface:initialize()

		table.insert(actor_interfaces, new_interface)
		table.insert(all_bottles, new_interface)

		self.bottle_count = self.bottle_count + 1
	end

	return scene_table
end