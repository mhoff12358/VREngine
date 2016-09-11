from scene_system_ import *
import collections

#def ActorInit(self):
#    super(Actor, self).__init__()
#    self.EmbedSelf(self)
#Actor.__init__ = ActorInit

class SceneSystemGlobals(object):
    query_registry = 'a'
    command_registry = 'a'

def ParseResources(resources):
    SceneSystemGlobals.query_registry = resources["query_registry"]
    SceneSystemGlobals.command_registry = resources["command_registry"]
 
class DelegatingActor(Actor):
    def DefaultHandleCommand(self, command_args):
        print("Failed to provide a handler for command args of type", command_args.Type())

    @classmethod
    def GetDefaultHandleCommand(cls):
        return cls.DefaultHandleCommand

    @classmethod
    def GetDefaultDelegation(cls):
        return collections.defaultdict(DelegatingActor.GetDefaultHandleCommand)

    def HandleCommand(self, command_args):
        self.command_delegation[command_args.Type()](self, command_args)
       
DelegatingActor.command_delegation = DelegatingActor.GetDefaultDelegation()

def delegate_for_command(delegation):
    def sub_fn(command_type):
        def sub_sub_fn(fn):
            delegation[command_type] = fn
            return fn
        return sub_sub_fn
    return sub_fn