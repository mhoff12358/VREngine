import scene_system as sc

class MutableGraphicalObject(sc.DelegatingActor):
	command_delegation = sc.DelegatingActor.GetDefaultDelegation()
	delegater = sc.delegate_for_command(command_delegation)

	def __init__(self):
		super(sc.DelegatingActor, self).__init__()
		self.EmbedSelf(self)
		
	@delegater(sc.CommandType.ADDED_TO_SCENE)
	def HandleAddToScene(self, args):
		scene = self.GetScene()

		latest_command = scene.FrontOfCommands()
		latest_command = scene.MakeCommandAfter(
			latest_command,
			sc.Target(scene.FindByName("IOInterface")),
			sc.IOListenerRegistration(
				True, self.id, sc.ListenerId.KEY_TOGGLE, ord(' ')))

		self.graphics_resources = sc.GraphicsResources.GetGraphicsResources(scene)

		model_generator = sc.ModelGenerator(sc.VertexType.texture, sc.D3DTopology.TRIANGLELIST)
		model_generator.AddVertexBatch(sc.Vertices(
			sc.VertexType.texture,
			sc.VectorArrayFloat5((
				sc.ArrayFloat5((-1, -1, 0, 0, 0)),
				sc.ArrayFloat5((-1,  1, 0, 0, 1)),
				sc.ArrayFloat5(( 1, -1, 0, 1, 0)),
				sc.ArrayFloat5(( 1, -1, 0, 1, 0)),
				sc.ArrayFloat5((-1,  1, 0, 0, 1)),
				sc.ArrayFloat5(( 1,  1, 0, 1, 1)),
				))))
		model_generator.SetParts(sc.MapStringToModelSlice({"Square":sc.ModelSlice(6, 0)}))
		model_generator.Finalize(
			self.graphics_resources.GetDeviceInterface(),
			self.graphics_resources.GetEntityHandler(),
			sc.ModelStorageDescription(False, True, False))

		self.graphics_object_id = scene.AddAndConstructGraphicsObject()
		self.graphics_object_model_name = sc.ResourceIdentifier.GetNewModelName("mutable_square")
		latest_command = scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.graphics_object_id),
			sc.CreateGraphicsObject(
				"basic",
				sc.VectorEntitySpecification((
					sc.EntitySpecification("square") \
						.SetModel(sc.ModelDetails(
							sc.ModelIdentifier(self.graphics_object_model_name),
							model_generator))
						.SetShaders(sc.ShaderDetails(
							"textured.hlsl",
							sc.VertexType.texture,
							sc.ShaderStages.Vertex().And(sc.ShaderStages.Pixel())))
						.SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((0, 0, 0)), sc.VectorFloat((1,)))))
						.SetTextures(sc.VectorIndividualTextureDetails((sc.IndividualTextureDetails("terrain.png", sc.ShaderStages.All(), 0, 0),)))
						.SetComponent("square")
	,)),
				 sc.VectorComponentInfo((sc.ComponentInfo("", "square"),))))
		latest_command = scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.graphics_object_id),
			sc.PlaceComponent(
				"square",
				sc.Pose(sc.Location(0, 0, -3))))

	@delegater(sc.IOInterfaceCommand.LISTEN_KEY_TOGGLE)
	def HandleKeyToggle(self, args):
		model_mutation = sc.ModelMutation()
		model_mutation.AddVertexBlock(
			5,
			sc.Vertices(
				sc.VertexType.texture,
				sc.VectorArrayFloat5((
					sc.ArrayFloat5((-2,  2, 0, 0, 1)),
					sc.ArrayFloat5(( 2,  2, 0, 1, 1)),
					))))
		self.graphics_resources.GetEntityHandler().AddModelMutation(self.graphics_object_model_name, model_mutation)
		print(args.key)
