require "class"
require "quaternion"

Positionable = Class:CHILD("Positionable"):DEF({
	CONSTRUCTOR = function (self, args)
		self.name = args["name"]
		self.set_component_transformation = args["set_component_transformation"]
		self:position(args["location"], args["orientation"], args["scale"])
		Class.CONSTRUCTOR(self, Class:NIL_ARGS(args, { "location", "orientation", "scale", "name", "set_component_transformation" }))
	end,
	compute_model_matrix = function (self)
		return {{
				["matrix_type"] = "scale",
				["scale"] = self.scale,
			},{
				["matrix_type"] = "quaternion_rotation",
				["quaternion"] = self.orientation,
			},{
				["matrix_type"] = "translation",
				["x"] = self.location[1],
				["y"] = self.location[2],
				["z"] = self.location[3],
		}}
	end,
	position = function (self, location, orientation, scale)
		if location ~= nil then
			self.location = location
		else
			self.location = { 0, 0, 0 }
		end
		if orienation ~= nil then
			self.orientation = orientation
		else
			self.orientation = quaternion.identity()
		end
		if scale ~= nil then
			self.scale = scale
		else
			self.scale = { 1, 1, 1 }
		end
		self.set_component_transformation(self.name, self:compute_model_matrix())
	end,
	sparse_position = function (self, location, orientation, scale)
		if location ~= nil then
			self.location = location
		end
		if orientation ~= nil then
			self.orientation = orientation
		end
		if scale ~= nil then
			self.scale = scale
		end
		self.set_component_transformation(self.name, self:compute_model_matrix())
	end,
})