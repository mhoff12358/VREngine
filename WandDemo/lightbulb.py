import scene_system as sc
import shader_helper, light

class LightBulb(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    num_motion_modifiers = {
        ord('1'): (2, -1),
        ord('2'): (2, 1),
        ord('3'): (0, -1),
        ord('4'): (0, 1),
        ord('5'): (1, -1),
        ord('6'): (1, 1),
    }

    def __init__(self, light_system_name = "", light_number = 0, color = sc.Color(1, 1, 1, 1)):
        super(sc.DelegatingActor, self).__init__()
        self.EmbedSelf(self)
        self.location = sc.Location(1, 0, 1)
        self.motion = sc.Location(0, 0, 0)
        self.power_motion = 0
        self.motion_scale = 0
        self.move_speed = 0.003
        self.light_system_name = light_system_name
        self.light_number = light_number
        self.color = color

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddToScene(self, args):
        # Registers for IO
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(), sc.Target(
                self.scene.FindByName("IOInterface")), sc.IOListenerRegistration(
                True, self.id, sc.ListenerId.KEY_TOGGLE, sc.VectorUnsignedChar(
                    (ord('1'), ord('2'), ord('3'), ord('4'), ord('5'), ord('6'), ord('7'), ord('8')))))
        self.scene.AddActorToGroup(self.id, self.scene.FindByName("TickRegistry"))

        # Gets the entity handler to access the light system
        graphics_resources = self.scene.AskQuery(
            sc.Target(
                self.scene.FindByName("GraphicsResources")), sc.QueryArgs(
                sc.GraphicsResourceQuery.GRAPHICS_RESOURCE_REQUEST)).GetGraphicsResources()
        self.entity_handler = graphics_resources.GetEntityHandler()

        self.graphics_id = self.scene.AddAndConstructGraphicsObject().id
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            sc.Target(self.graphics_id),
            sc.CreateGraphicsObject(
                "basic",
                sc.VectorEntitySpecification((
                    sc.EntitySpecification("LightBulb")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier("cube.obj"),
                        sc.OutputFormat(
                            sc.ModelModifier(
                                sc.ArrayInt3((0, 1, 2)),
                                sc.ArrayFloat3((1, 1, 1)),
                                sc.ArrayBool2((False, True))),
                            sc.VertexType.all,
                            False)))
                    .SetShaders(shader_helper.ShaderHelper.Default(pixel_shader_name = "ps_solidcolor", lighting = False))
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((self.color.AsVector(),)))
                    .SetComponent("Cube"),)),
                sc.VectorComponentInfo((sc.ComponentInfo("", "Cube"),))))
        self.light = light.PointLight(location = self.location, color = self.color, light_system_name = self.light_system_name, number = 0, scene = self.scene)
        self.PushPose()

    def GetLight(self):
        light_system = self.entity_handler.MutableLightSystem(self.light_system_name)
        point_lights = light_system.MutablePointLights()
        point_light = point_lights[self.light_number]
        return point_light

    @delegater.RegisterCommand(sc.IOInterfaceCommand.LISTEN_KEY_TOGGLE)
    def HandleKeyToggle(self, args):
        if args.key in self.num_motion_modifiers:
            motion_modifier = self.num_motion_modifiers[args.key]
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
        elif args.key in (ord('7'), ord('8')):
            self.power_motion += (1 if args.pressed else -1) * (1 if args.key == ord('7') else -1) * 0.005

    @delegater.RegisterCommand(sc.InputCommand.TICK)
    def HandleTick(self, args):
        if self.motion_scale:
            self.location += self.motion * self.motion_scale * self.move_speed * args.duration
            self.PushPose()
        if self.power_motion:
            self.color.a += self.power_motion
            self.PushColor()

    def PushPose(self):
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            sc.Target(self.graphics_id),
            sc.PlaceComponent("Cube", sc.Pose(self.location, sc.Scale(0.1, 0.1, 0.1))))
        self.light.SetLocation(self.location)

    def PushColor(self):
        self.light.SetColor(self.color)
