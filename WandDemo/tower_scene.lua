function create_scene ()
	scene_table = {}

	function scene_table.initialize(self)
		self.bottle_count = 1
		self.add_actor("wall.lua", "wall")

		self.add_listener("space_down", self.cpp_interface)
	end

	function scene_table.space_down(self)
		self.add_actor("bottle.lua", "bottle" .. tostring(self.bottle_count))
		self.bottle_count = self.bottle_count + 1
	end

	return scene_table
end