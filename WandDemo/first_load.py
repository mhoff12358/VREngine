import scene_system as sc
import collections
import scripts.player
import scripts.mutable_graphical_object

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

	graphics_object_id = scripts.mutable_graphical_object.MutableGraphicalObject()
	scene.AddActor(graphics_object_id)

	import code
	a = globals()
	a.update(locals())
	code.interact(local = a)

#	graphics_object_id = scene.AddAndConstructGraphicsObject()
#	latest_command = scene.MakeCommandAfter(
#		scene.FrontOfCommands(),
#		sc.Target(graphics_object_id),
#		sc.CreateGraphicsObject(
#			"basic",
#			sc.VectorEntitySpecification((
#				sc.EntitySpecification("square") \
#					.SetModel(sc.ModelDetails(sc.ModelIdentifier("square.obj|Square")))
#					.SetShaders(sc.ShaderDetails(
#						"texturedspecularlightsource.hlsl",
#						sc.VertexType.all,
#						sc.ShaderStages.Vertex().And(sc.ShaderStages.Pixel())))
#					.SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((0, 0, 0)), sc.VectorFloat((1,)))))
#					.SetTextures(sc.VectorIndividualTextureDetails((sc.IndividualTextureDetails("terrain.png", sc.ShaderStages.All(), 0, 0),)))
#					.SetComponent("square")
#,)),
# 		   sc.VectorComponentInfo((sc.ComponentInfo("", "square"),))))
#	latest_command = scene.MakeCommandAfter(
#		latest_command,
#		sc.Target(graphics_object_id),
#		sc.PlaceComponent(
#			"square",
#			sc.Pose(sc.Location(0, 0, -3), sc.Quaternion.RotationAboutAxis(sc.AxisID.x, 3.14/2.0))))

	p = scripts.player.Player()
	player_id = scene.AddActor(p)
	return player_id

def dump_thing(thing):
	print(thing)