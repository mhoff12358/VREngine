
Class = {
  NAME = "Class",
  
  NEW = function(cls, args)
    new_object = {}
    setmetatable(new_object, cls)
    cls.CONSTRUCTOR(new_object, args)
    return new_object
  end,

  __index = function(instance, key)
    return getmetatable(instance)[key]
  end,

  CHILD = function(cls, name)
    new_cls = {}
	for key, value in pairs(cls) do
		new_cls[key] = value
	end
	new_cls.NAME = name
	new_cls.CLS = new_cls
	new_cls.PARENTS = {}
	new_cls.PARENTS[cls.NAME] = cls

	return new_cls
  end,

  SUPER = function(cls, name)
	return cls.PARENTS[name]
  end,

  INHERIT_FROM = function(cls, parent_class)
    for key, value in pairs(parent_class) do
      cls[key] = value
    end
    return cls
  end,

  DEF = function(cls, new_class_attributes)
    for key, value in pairs(new_class_attributes) do
      cls[key] = value
    end
    return cls
  end,

  CONSTRUCTOR = function(self, args)
    if args then
      for key, value in pairs(args) do
        self[key] = value
      end
    end
    return self
  end,

  NIL_ARGS = function(cls, args, keys_to_nil)
	for _, value in pairs(keys_to_nil) do
		args[value] = nil
	end
	return args
  end,
}

Class.CLS = Class

--[[
A = Class:DEF({
  fun = function(self)
    return self.stuff
  end,
})

a1 = A:NEW({stuff = 12})

B = Class:DEF({
  fun = function(self)
    return self.stuff + 5
  end,
}):INHERIT_FROM(A)

b2 = B:NEW({stuff = 19})

print(a1:fun())
print(b2:fun())
]]--
