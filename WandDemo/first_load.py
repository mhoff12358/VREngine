import scene_system as sc
import collections
import scripts.player
import scripts.mutable_graphical_object
import scripts.kinect_body_display
import scripts.draggable_object
import scripts.path_draggable_object

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

class Line(object):
	def __init__(self, p0, p1):
		self.p0 = p0
		self.p1 = p1

	def at(self, t):
		return (1-t) * self.p0 + t * self.p1

def first_load(resources):
	print("STARTING FIRST LOAD")
	sc.ParseResources(resources)

	scene = resources["scene"]

	#graphics_object = scripts.mutable_graphical_object.MutableGraphicalObject()
	#scene.AddActor(graphics_object)

	scene.AddActor(scripts.path_draggable_object.PathDraggableObject(
		(Line(sc.Location(0, 0, 0), sc.Location(0, 2, 0)),)))

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