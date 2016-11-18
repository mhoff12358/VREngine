from scene_system_ import *
from .actor import *
from .delegating_actor import *

class SceneSystemGlobals(object):
	query_registry = 'a'
	command_registry = 'a'

def ParseResources(resources):
	SceneSystemGlobals.query_registry = resources["query_registry"]
	SceneSystemGlobals.command_registry = resources["command_registry"]
