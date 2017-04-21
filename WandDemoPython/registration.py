from scene_system_ import *
import enum

class SceneSystemGlobals(object):
    query_registry = RegistryMap.GetQueryRegistry()
    command_registry = RegistryMap.GetCommandRegistry()

def ParseResources(resources):
    import pdb; pdb.set_trace()
    resources["query_registry"].AddRegistrationsFrom(SceneSystemGlobals.query_registry)
    SceneSystemGlobals.query_registry.AddRegistrationsFrom(resources["query_registry"])
    SceneSystemGlobals.query_registry = resources["query_registry"]
    resources["command_registry"].AddRegistrationsFrom(SceneSystemGlobals.command_registry)
    SceneSystemGlobals.command_registry.AddRegistrationsFrom(resources["command_registry"])
    SceneSystemGlobals.command_registry = resources["command_registry"]
    RegistryMap.SetCommandRegistry(resources["command_registry"])

def AddCommandType(new_command_name: str):
    import pdb; pdb.set_trace()
    new_val = SceneSystemGlobals.command_registry.Register(new_command_name)
    print("Registering new command:", new_command_name, new_val)
    return new_val

def AddQueryType(new_query_name: str):
    return SceneSystemGlobals.query_registry.Register(new_query_name)


class CommandRegistration(enum.IntEnum):
    def __new__(cls, *args):
        obj = int.__new__(cls)
        obj._value_ = None
        return obj

    def __init__(self, *args):
        import pdb; pdb.set_trace()
        self._value_ = AddCommandType(str(self.__class__) + self.name)

    def __eq__(self, other):
        return self is other

    def __ne__(self, other):
        return not self == other

    def __hash__(self, **kwargs):
        return super().__hash__(**kwargs)

class QueryRegistration(enum.IntEnum):
    def __new__(cls, *args):
        obj = int.__new__(cls)
        obj._value_ = None
        return obj

    def __init__(self, *args):
        self._value_ = AddQueryType(str(self.__class__) + self.name)

    def __eq__(self, other):
        return self is other

    def __ne__(self, other):
        return not self == other

    def __hash__(self, **kwargs):
        return super().__hash__(**kwargs)
