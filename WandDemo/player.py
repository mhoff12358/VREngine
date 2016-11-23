import scene_system as sc


class Player(sc.DelegatingActor):
    command_delegation = sc.DelegatingActor.GetDefaultDelegation()
    delegater = sc.delegate_for_command(command_delegation)

    motion_modifiers = {
        ord('W'): (2, -1),
        ord('S'): (2, 1),
        ord('A'): (0, -1),
        ord('D'): (0, 1),
        ord('Q'): (1, -1),
        ord('E'): (1, 1),
    }

    def __init__(self):
        super(sc.DelegatingActor, self).__init__()
        self.EmbedSelf(self)
        self.pitch = 0.0
        self.yaw = 0.0
        self.rotation = sc.Quaternion.Identity()
        self.location = sc.Location(0, 0, 3)
        self.motion = sc.Location(0, 0, 0)
        self.motion_scale = 0
        self.move_speed = 0.003

    @delegater(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddToScene(self, args):
        print("Player added to the scene")
        scene = self.GetScene()

        # Registers for IO
        scene.MakeCommandAfter(
            scene.FrontOfCommands(), sc.Target(
                scene.FindByName("IOInterface")), sc.IOListenerRegistration(
                True, self.id, sc.ListenerId.KEY_TOGGLE, sc.VectorUnsignedChar(
                    (ord('W'), ord('A'), ord('S'), ord('D'), ord('Q'), ord('E'),))))
        scene.MakeCommandAfter(
            scene.FrontOfCommands(),
            sc.Target(scene.FindByName("IOInterface")),
            sc.IOListenerRegistration(
                True, self.id, sc.ListenerId.MOUSE_MOTION))
        scene.AddActorToGroup(self.id, scene.FindByName("TickRegistry"))

        # Gets the entity handler to access the camera
        graphics_resources = scene.AskQuery(
            sc.Target(
                scene.FindByName("GraphicsResources")), sc.QueryArgs(
                sc.GraphicsResourceQuery.GRAPHICS_RESOURCE_REQUEST)).GetGraphicsResources()
        self.entity_handler = graphics_resources.GetEntityHandler()

    @delegater(sc.IOInterfaceCommand.LISTEN_MOUSE_MOTION)
    def HandleMouseMovement(self, args):
        self.yaw += args.motion[0] * -0.002
        self.yaw = self.yaw % (3.14 * 2)
        self.pitch += args.motion[1] * -0.002
        self.pitch = self.pitch % (3.14 * 2)
        self.rotation = sc.Quaternion.RotationAboutAxis(
            sc.AxisID.y, self.yaw) * sc.Quaternion.RotationAboutAxis(sc.AxisID.x, self.pitch)
        self.PushPose()

    @delegater(sc.IOInterfaceCommand.LISTEN_KEY_TOGGLE)
    def HandleKeyToggle(self, args):
        if args.key in self.motion_modifiers:
            motion_modifier = self.motion_modifiers[args.key]
            location_thing = tuple(
                motion_modifier[1] * (1 if args.pressed else -1)
                if i == motion_modifier[0] else 0 for i in range(3))
            self.motion += sc.Location(*location_thing)
            mag = pow(abs(self.motion[0]) +
                      abs(self.motion[1]) +
                      abs(self.motion[2]), 0.5)
            if mag:
                self.motion_scale = 1 / mag
            else:
                self.motion_scale = 0

    @delegater(sc.InputCommand.TICK)
    def HandleTick(self, args):
        if self.motion_scale:
            self.location += self.motion.Rotate(
                self.rotation) * self.motion_scale * self.move_speed * args.duration

    def PushPose(self):
        self.entity_handler.MutableCamera("player_head").SetPose(
            sc.Pose(self.location, self.rotation))
