import scene_system as sc

class Player(sc.DelegatingActor):
	command_delegation = sc.DelegatingActor.GetDefaultDelegation()
	delegater = sc.delegate_for_command(command_delegation)

	def __init__(self):
		super(sc.DelegatingActor, self).__init__()
		self.EmbedSelf(self)
		self.pitch = 0.0
		self.yaw = 0.0
		
	@delegater(sc.CommandType.ADDED_TO_SCENE)
	def HandleAddToScene(self, args):
		print("Player added to the scene")
		scene = self.GetScene()

		# Registers for IO
		scene.MakeCommandAfter(
			scene.FrontOfCommands(),
			sc.Target(scene.FindByName("IOInterface")),
			sc.IOListenerRegistration(
				True, self.id, sc.ListenerId.KEY_TOGGLE, ord('B')))
		scene.MakeCommandAfter(
			scene.FrontOfCommands(),
			sc.Target(scene.FindByName("IOInterface")),
			sc.IOListenerRegistration(
				True, self.id, sc.ListenerId.MOUSE_MOTION))

		# Gets the entity handler to access the camera
		graphics_resources = scene.AskQuery(
			sc.Target(scene.FindByName("GraphicsResources")),
			sc.QueryArgs(sc.GraphicsResourceQuery.GRAPHICS_RESOURCE_REQUEST)).GetGraphicsResources()
		self.entity_handler = graphics_resources.GetEntityHandler()

	@delegater(sc.IOInterfaceCommand.LISTEN_MOUSE_MOTION)
	def HandleMouseMovement(self, args):
		self.yaw += args.motion[0] * -0.002
		self.yaw = self.yaw % (3.14 * 2)
		self.pitch += args.motion[1] * -0.002
		self.pitch = self.pitch % (3.14 * 2)
		self.PushPose()

	@delegater(sc.IOInterfaceCommand.LISTEN_KEY_TOGGLE)
	def HandleKeyToggle(self, args):
		print(args.key)

	def PushPose(self):
		self.entity_handler.MutableCamera("player_head").SetPose(sc.Pose(sc.Location(0, 0, 3), sc.Quaternion.RotationAboutAxis(sc.AxisID.y, self.yaw) * sc.Quaternion.RotationAboutAxis(sc.AxisID.x, self.pitch)))