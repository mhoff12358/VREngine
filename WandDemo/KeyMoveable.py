import scene_system as sc

class KeyMoveable(sc.DelegatingActor[sc.Actor]):
    delegater = sc.Delegater(sc.DelegatingActor[sc.Actor])

    motion_modifiers = {
        ord('I'): (2, -1),
        ord('K'): (2, 1),
        ord('J'): (0, -1),
        ord('L'): (0, 1),
        ord('U'): (1, -1),
        ord('O'): (1, 1),
    }

    def __init__(self):
        super().__init__()
        self.location = sc.Location(0, 0, 3)
        self.motion = sc.Location(0, 0, 0)
        self.motion_scale = 0
        self.move_speed = 0.003

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddToScene(self, args):
        scene = self.GetScene()

        # Registers for IO
            scene.MakeCommandAfter(
                scene.FrontOfCommands(), sc.Target(
                    scene.FindByName("IOInterface")), sc.IOListenerRegistration(
                    True, self.id, sc.ListenerId.KEY_TOGGLE, sc.VectorUnsignedChar(
                        (ord('I'), ord('K'), ord('J'), ord('L'), ord('U'), ord('O'), ord('P')))))
            scene.AddActorToGroup(self.id, scene.FindByName("TickRegistry"))

        # Gets the entity handler to access the camera
        graphics_resources = scene.AskQuery(
            sc.Target(
                scene.FindByName("GraphicsResources")), sc.QueryArgs(
                sc.GraphicsResourceQuery.GRAPHICS_RESOURCE_REQUEST)).GetGraphicsResources()
        self.entity_handler = graphics_resources.GetEntityHandler()

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
        sc.Pose(self.location)
