import scene_system as sc

class DraggableObject(sc.DelegatingActor):
    command_delegation = sc.DelegatingActor.GetDefaultDelegation()
    delegater = sc.delegate_for_command(command_delegation)

    def __init__(self, collision_shapes, starting_pose = sc.Pose()):
        super().__init__()

        self.current_pose = starting_pose
        self.collision_shapes, self.collision_shape_offsets = zip(*collision_shapes)
        for shape in self.collision_shapes:
            shape.SetPose(self.current_pose)

        self.controller_pose_delta = sc.Pose()

    def ProposePose(self, proposed_pose):
        self.current_pose = proposed_pose

    @delegater(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddToScene(self, args):
        latest_command = self.scene.FrontOfCommands()
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
            sc.AddGrabbableObject(
                self.id,
                sc.VectorCollisionShape(self.collision_shapes)))
        self.collision_shapes = None

    @delegater(sc.HeadsetInterfaceCommand.LISTEN_CONTROLLER_MOVEMENT)
    def HandleControllerMovementWhileGrabbed(self, args):
        self.scene = self.GetScene()
        proposed_pose = self.controller_pose_delta.ApplyAfter(args.position)
        self.ProposePose(proposed_pose)

    @delegater(sc.GrabbableObjectCommand.OBJECT_GRABBED)
    def HandleGrabbed(self, args):
        if args.held:
            self.controller_pose_delta = args.position.Delta(self.current_pose)
            self.scene.MakeCommandAfter(
                self.scene.FrontOfCommands(),
                sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
                sc.EnDisableGrabbableObject(self.id, False))
        else:
            self.scene.MakeCommandAfter(
                self.scene.FrontOfCommands(),
								sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
								sc.ReposeGrabbableObject(self.id, self.current_pose))
            self.scene.MakeCommandAfter(
                self.scene.FrontOfCommands(),
								sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
								sc.EnDisableGrabbableObject(self.id, True))
