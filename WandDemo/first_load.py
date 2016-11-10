import scene_system as sc
import collections
import scripts.player
import scripts.mutable_graphical_object
import scripts.kinect_body_display
import scripts.draggable_object

class DummyActor(sc.DelegatingActor):
	command_delegation = sc.DelegatingActor.GetDefaultDelegation()

	def __init__(self):
		super(DummyActor, self).__init__()
		self.EmbedSelf(self)

	def AnswerQuery(self, query_args):
		print("RECEIVED QUERY ARGS OF TYPE", query_args.Type())
		return QueryResult(query_args.Type())

	def HandleTICK(self, command_args):
		print("RECEIVED COMMAND OF TYPE TICK", command_args.duration)
	command_delegation[sc.InputCommand.TICK] = HandleTICK

def first_load(resources):
	print("STARTING FIRST LOAD")
	sc.ParseResources(resources)

	scene = resources["scene"]

	#graphics_object = scripts.mutable_graphical_object.MutableGraphicalObject()
	#scene.AddActor(graphics_object)

	scene.AddActor(scripts.draggable_object.DraggableObject())

	#body_display = scripts.kinect_body_display.KinectBodyDisplay()
	#scene.AddActor(body_display)

	import code
	a = globals()
	a.update(locals())
	#code.interact(local = a)

	p = scripts.player.Player()
	player_id = scene.AddActor(p)
	return player_id

def dump_thing(thing):
	print(thing)