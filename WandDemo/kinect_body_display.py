import scene_system as sc

class KinectBodyDisplay(sc.DelegatingActor):
	command_delegation = sc.DelegatingActor.GetDefaultDelegation()
	delegater = sc.delegate_for_command(command_delegation)

	def __init__(self):
		super(sc.DelegatingActor, self).__init__()
		self.EmbedSelf(self)

		self.latest_body_id = None
		
	@delegater(sc.CommandType.ADDED_TO_SCENE)
	def HandleAddToScene(self, args):
		scene = self.GetScene()

		latest_command = scene.FrontOfCommands()
		latest_command = scene.MakeCommandAfter(
			latest_command,
			sc.Target(scene.FindByName("KinectInterface")),
			sc.ListenForBodies(self.id, True)
			)

		self.body_lookup = sc.KinectInterface.GetBodyLookup(scene)
		scene.AddActorToGroup(self.id, scene.FindByName("TickRegistry"))

		self.graphics_object_id = scene.AddAndConstructGraphicsObject().id
		latest_command = scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.graphics_object_id),
			sc.CreateGraphicsObject(
				"basic",
				sc.VectorEntitySpecification((
					sc.EntitySpecification("Square") \
						.SetModel(sc.ModelDetails(
							sc.ModelIdentifier("square.obj|Square")))
						.SetShaders(sc.ShaderDetails(
							"texturednormaledshader.hlsl",
							sc.VertexType.all,
							sc.ShaderStages.Vertex().And(sc.ShaderStages.Pixel())))
						.SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((0, 0, 0)), sc.VectorFloat((1,)))))
						.SetTextures(sc.VectorIndividualTextureDetails((sc.IndividualTextureDetails("terrain.png", sc.ShaderStages.All(), 0, 0),)))
						.SetComponent("Square")
	,)),
				 sc.VectorComponentInfo((sc.ComponentInfo("", "Square"),))))
		latest_command = scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.graphics_object_id),
			sc.PlaceComponent(
				"Square",
				sc.Pose(sc.Location(0, 0, -3), sc.Quaternion.RotationAboutAxis(sc.AxisID.x, 3.14/2.0))))

		self.latest_head_tracked_conf = None

	@delegater(sc.KinectInterfaceCommand.BODIES_DISCOVERED)
	def HandleBodiesDiscovered(self, args):
		print("BODIES FOUND")
		tracking_ids_found = args.tracking_ids
		if len(tracking_ids_found):
			self.latest_body_id = tracking_ids_found[0]

	@delegater(sc.InputCommand.TICK)
	def HandleTimeTick(self, args):
		if self.latest_body_id is not None:
			latest_body = self.body_lookup.GetBody(self.latest_body_id)
			if latest_body.filled:
				self.UseBody(latest_body)
			else:
				pass

	def UseBody(self, body):
		for joint in body.joints:
			if joint.joint_type == sc.JointType.Head:
				if joint.tracking_state != self.latest_head_tracked_conf:
					self.latest_head_tracked_conf = joint.tracking_state
					#print("Head status: ", joint.tracking_state)
				#print(joint.position.X, joint.position.Y, joint.position.Z)
				scene = self.GetScene()
				scene.MakeCommandAfter(
					scene.FrontOfCommands(),
					sc.Target(self.graphics_object_id),
					sc.PlaceComponent(
						"Square",
						sc.Pose(sc.Location(joint.position.X, joint.position.Y, joint.position.Z*(-3)), sc.Quaternion.RotationAboutAxis(sc.AxisID.x, 3.14/2.0))))