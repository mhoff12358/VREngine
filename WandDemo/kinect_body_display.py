import scene_system as sc

class KinectBodyDisplay(sc.DelegatingActor):
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
			sc.Target(scene.FindByName("KinectInterface")),
			sc.ListenForBodies(self.id, True)
			)

	@delegater(sc.KinectInterfaceCommand.BODIES_DISCOVERED)
	def HandleBodiesDiscovered(self, args):
		print("BODIES FOUND")