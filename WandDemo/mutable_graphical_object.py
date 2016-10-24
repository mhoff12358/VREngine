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

		self.graphics_object_id = scene.AddAndConstructGraphicsObject()
		latest_command = scene.MakeCommandAfter(
			scene.FrontOfCommands(),
			sc.Target(self.graphics_object_id),
			sc.CreateGraphicsObject(
				"basic",
				sc.VectorEntitySpecification((
					sc.EntitySpecification("square") \
						.SetModel(sc.ModelDetails(
							sc.ModelIdentifier(sc.ResourceIdentifier.GetNewModelName("mutable_square")),
							model_generator,
							sc.ModelStorageDescription(False, True, False)))
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


	def PushPose(self):
		self.entity_handler.MutableCamera("player_head").SetPose(sc.Pose(sc.Quaternion.RotationAboutAxis(sc.AxisID.y, self.yaw)))
