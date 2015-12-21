require "cpp_interface"
require "positionable"

OwnsPositionables = CppInterface:CHILD("OwnsPositionables"):DEF({
	CONSTRUCTOR = function (self, args)
		CppInterface.CONSTRUCTOR(self, args)
		self.positionables = {}
	end,
	add_positionable = function (self, component_name)
		self.positionables[component_name] = Positionable:NEW({
				set_component_transformation = self.callbacks.set_component_transformation,
				name = component_name
			})
	end,
	get_positionable = function (self, component_name)
		return self.positionables[component_name]
	end,
})