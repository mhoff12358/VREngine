from scene_system_ import *
import enum

class SceneSystemGlobals(object):
    query_registry = RegistryMap()
    command_registry = RegistryMap()

def ParseResources(resources):
    resources["query_registry"].AddRegistrationsFrom(SceneSystemGlobals.query_registry)
    SceneSystemGlobals.query_registry = resources["query_registry"]
    resources["command_registry"].AddRegistrationsFrom(SceneSystemGlobals.command_registry)
    SceneSystemGlobals.command_registry = resources["command_registry"]

def AddCommandType(new_command_name: str):
    return SceneSystemGlobals.command_registry.Register(new_command_name)

def AddQueryType(new_query_name: str):
    return SceneSystemGlobals.query_registry.Register(new_query_name)


class CommandRegistration(enum.Enum):
    def __init__(self, *args):
        self.id = AddCommandType(str(self.__class__) + self.name)

    def __int__(self):
        return self.id

class QueryRegistration(enum.Enum):
    def __init__(self, *args):
        self.id = AddQueryType(str(self.__class__) + self.name)

    def __int__(self):
        return self.id