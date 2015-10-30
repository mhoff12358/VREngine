function create_actor()
	actor_table = {}
	
	actor_table.interaction_callbacks = {}
	function actor_table.interaction_callbacks.initialize (self)
	end

	return actor_table
end