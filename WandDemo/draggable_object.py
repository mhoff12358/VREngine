import scene_system as sc

class DraggableObject(sc.DelegatingActor):
	command_delegation = sc.DelegatingActor.GetDefaultDelegation()
	delegater = sc.delegate_for_command(command_delegation)

	def __init__(self):
		super().__init__()

		self.current_location = sc.Location(0, 1, 0)
		self.controller_location_delta = sc.Location(0, 0, 0)
		self.radius = 0.1

	def PlaceSelf(self, latest_command):
		return self.scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.graphics_object_id),
			sc.PlaceComponent(
				"Sphere",
				sc.Pose(self.current_location, sc.Scale(self.radius))))

	def SetColor(self, rgba):
		pass
		
	@delegater(sc.CommandType.ADDED_TO_SCENE)
	def HandleAddToScene(self, args):
		latest_command = self.scene.FrontOfCommands()

		self.graphics_object_id = self.scene.AddAndConstructGraphicsObject().id
		latest_command = self.scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.graphics_object_id),
			sc.CreateGraphicsObject(
				"basic",
				sc.VectorEntitySpecification((
					sc.EntitySpecification("sphere") \
						.SetModel(sc.ModelDetails(
							sc.ModelIdentifier("sphere.obj|Sphere")))
						.SetShaders(sc.ShaderDetails(
							"solidcolor.hlsl",
							sc.VertexType.location,
							sc.ShaderStages.Vertex().And(sc.ShaderStages.Pixel())))
						.SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((0.5, 0, 0)),)))
						.SetComponent("Sphere"),)),
				 sc.VectorComponentInfo((sc.ComponentInfo("", "Sphere"),))))
		latest_command = self.PlaceSelf(latest_command)
		latest_command = self.scene.MakeCommandAfter(
			latest_command,
			sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
			sc.AddGrabbableObject(self.id, sc.VectorCollisionShape((sc.CollisionShape(self.current_location, self.radius),))))

	@delegater(sc.HeadsetInterfaceCommand.LISTEN_CONTROLLER_MOVEMENT)
	def HandleControllerMovementWhileGrabbed(self, args):
		self.scene = self.GetScene()
		self.current_location = args.position.location + self.controller_location_delta
		self.PlaceSelf(self.scene.FrontOfCommands())

	@delegater(sc.GrabbableObjectCommand.OBJECT_GRABBED)
	def HandleGrabbed(self, args):
		if args.held:
			self.controller_location_delta = self.current_location - args.position.location
			self.scene.MakeCommandAfter(
				self.scene.FrontOfCommands(),
				sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
				sc.RemoveGrabbableObject(self.id))
		else:
			self.scene.MakeCommandAfter(
				self.scene.FrontOfCommands(),
				sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
				sc.AddGrabbableObject(self.id, sc.VectorCollisionShape((sc.CollisionShape(self.current_location, self.radius),))))
