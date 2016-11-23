from scene_system_ import *
from . import actor
import collections, copy

class DelegatingActor(actor.Actor):
	def DefaultHandleCommand(self, command_args):
		print("Failed to provide a handler for command args of type", command_args.Type())

	@classmethod
	def GetDefaultHandleCommand(cls):
		return cls.DefaultHandleCommand

	@classmethod
	def GetDefaultDelegation(cls):
		return copy.copy(cls.command_delegation)

	def HandleCommand(self, command_args):
		if command_args.Type() == CommandType.ADDED_TO_SCENE:
			self.scene = self.GetScene()
		self.command_delegation[command_args.Type()](self, command_args)

DelegatingActor.command_delegation = collections.defaultdict(DelegatingActor.GetDefaultHandleCommand)

def delegate_for_command(delegation):
	def sub_fn(command_type):
		def sub_sub_fn(fn):
			delegation[command_type] = fn
			return fn
		return sub_sub_fn
	return sub_fn
