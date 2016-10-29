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
				print(joint.position.X, joint.position.Y, joint.position.Z)