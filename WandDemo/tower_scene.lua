function create_scene ()
	scene_table = {}

	function scene_table.initialize(self)
		self.add_actor("wall.lua", "wall")
		self.add_actor("bottle.lua", "bottle")
	end

	return scene_table
end