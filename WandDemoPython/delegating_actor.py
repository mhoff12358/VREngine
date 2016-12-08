from scene_system_ import *
from . import actor
import collections, copy

class Delegater(object):
    def __init__(self, parent_cls = None):
        if parent_cls is None:
            self.command_delegation = collections.defaultdict(DelegatingActor.GetDefaultHandleCommand)
            self.query_delegation = collections.defaultdict(DelegatingActor.GetDefaultAnswerQuery)
        else:
            self.command_delegation = copy.copy(parent_cls.delegater.command_delegation)
            self.query_delegation = copy.copy(parent_cls.delegater.query_delegation)

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

class DelegatingActor(actor.Actor):
    def DefaultHandleCommand(self, command_args):
        print("Failed to provide a handler for command args of type", command_args.Type())

    @classmethod
    def GetDefaultHandleCommand(cls):
        return cls.DefaultHandleCommand

    def DefaultAnswerQuery(self, query_args):
        print("Failed to provide a handler for query args of type", query_args.Type())
        return None

    @classmethod
    def GetDefaultAnswerQuery(cls):
        return cls.DefaultAnswerQuery

    #@classmethod
    #def GetDefaultDelegation(cls):
    #    return copy.copy(cls.command_delegation)

    #@classmethod
    #def GetDefaultQueryDelegation(cls):
    #    return copy.copy(cls.query_delegation)

    def HandleCommand(self, command_args):
        if command_args.Type() == CommandType.ADDED_TO_SCENE:
            self.scene = self.GetScene()
        self.delegater.command_delegation[command_args.Type()](self, command_args)

    def AnswerQuery(self, query_args):
        return self.delegater.query_delegation[query_args.Type()](self, query_args)

DelegatingActor.delegater = Delegater()

