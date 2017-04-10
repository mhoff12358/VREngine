import scene_system as sc


class Player(sc.DelegatingActor[sc.Actor]):
    delegater = sc.Delegater(sc.DelegatingActor[sc.Actor])

    num_motion_modifiers = {
        ord('1'): (2, -1),
        ord('2'): (2, 1),
        ord('3'): (0, -1),
        ord('4'): (0, 1),
        ord('5'): (1, -1),
        ord('6'): (1, 1),
    }
    motion_modifiers = {
        ord('W'): (2, -1),
        ord('S'): (2, 1),
        ord('A'): (0, -1),
        ord('D'): (0, 1),
        ord('Q'): (1, -1),
        ord('E'): (1, 1),
    }

    def __init__(self, keyboard_and_mouse_controls = True):
        super().__init__()
        self.keyboard_and_mouse_controls = keyboard_and_mouse_controls
        self.EmbedSelf(self)
        self.pitch = 0.0
        self.yaw = 0.0
        self.rotation = sc.Quaternion.Identity()
        self.location = sc.Location(0, 0, 3)
        self.motion = sc.Location(0, 0, 0)
        self.motion_scale = 0
        self.move_speed = 0.003

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddToScene(self, args):
        scene = self.GetScene()

        # Registers for IO
        if self.keyboard_and_mouse_controls:
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

    @delegater.RegisterCommand(sc.IOInterfaceCommand.LISTEN_MOUSE_MOTION)
    def HandleMouseMovement(self, args):
        self.yaw += args.motion[0] * -0.002
        self.yaw = self.yaw % (3.14 * 2)
        self.pitch += args.motion[1] * -0.002
        self.pitch = self.pitch % (3.14 * 2)
        self.rotation = sc.Quaternion.RotationAboutAxis(
            sc.AxisID.y, self.yaw) * sc.Quaternion.RotationAboutAxis(sc.AxisID.x, self.pitch)

    @delegater.RegisterCommand(sc.IOInterfaceCommand.LISTEN_KEY_TOGGLE)
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

    @delegater.RegisterCommand(sc.InputCommand.TICK)
    def HandleTick(self, args):
        if self.motion_scale:
            self.location += self.motion.Rotate(
                self.rotation) * self.motion_scale * self.move_speed * args.duration
        self.PushPose()

    def PushPose(self):
        self.entity_handler.MutableCamera("player_head").SetPose(
            sc.Pose(self.location, self.rotation))
