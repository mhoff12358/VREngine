require "class"

CppInterface = Class:CHILD("CppInterface"):DEF({
	CONSTRUCTOR = function(self, args)
		Class.CONSTRUCTOR(self, args)
		self.callbacks = {}
		--self.cpp_interface is assumed to be filled by whatever function creates this.
	end,

	initialize = function (self)
	end,

	register_listener = function (self, registration_name)
		self.callbacks.add_listener(registration_name, self.cpp_interface)
	end,
})