from scene_system_ import *
from . import actor
import collections, copy

class Delegater(object):
    def __init__(self, parent_cls = None):
        if parent_cls is None:
            self.command_delegation = collections.defaultdict(self.GetDefaultHandleCommand)
            self.query_delegation = collections.defaultdict(self.GetDefaultAnswerQuery)
        else:
            self.command_delegation = copy.copy(parent_cls.delegater.command_delegation)
            self.query_delegation = copy.copy(parent_cls.delegater.query_delegation)

    def DefaultHandleCommand(self, command_args):
        pass

    @classmethod
    def GetDefaultHandleCommand(cls):
        return cls.DefaultHandleCommand

    def DefaultAnswerQuery(self, query_args):
        return None

    @classmethod
    def GetDefaultAnswerQuery(cls):
        return cls.DefaultAnswerQuery

    def RegisterCommand(self, command_type):
        def sub_fn(fn):
            self.command_delegation[command_type] = fn
            return fn
        return sub_fn

    def RegisterQuery(self, query_type):
        def sub_fn(fn):
            self.query_delegation[query_type] = fn
            return fn
        return sub_fn

class defaultdictwithargs(collections.defaultdict):
    def __missing__(self, key):
        if self.default_factory is None:
            raise KeyError( key )
        else:
            ret = self[key] = self.default_factory(key)
            return ret
def create_delegating_actor_class(actor_class):
    class DelegatingActor(actor_class):
        def HandleCommand(self, command_args):
            if command_args.Type() == CommandType.ADDED_TO_SCENE:
                self.scene = self.GetScene()
            if self.delegater.command_delegation[command_args.Type()](self, command_args):
                super().HandleCommand(command_args)

        def AnswerQuery(self, query_args):
            return self.delegater.query_delegation[query_args.Type()](self, query_args)
    DelegatingActor.delegater = Delegater()
    return DelegatingActor

DelegatingActor = defaultdictwithargs(create_delegating_actor_class)

