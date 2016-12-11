import scene_system as sc
import draggable_graphics, shell


class CannonBall(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)
    cannon_ball_size = 0.15

    def __init__(self, shell_attributes: shell.ShellAttributes, starting_pose: sc.Pose, direction: sc.Location):
        super().__init__()
        self.color = shell_attributes.color
        self.pose = starting_pose
        self.pose.scale = sc.Scale(self.cannon_ball_size)
        self.velocity = shell_attributes.power * direction.GetNormalized()

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        latest_command = self.LoadGraphicsResources(self.scene.FrontOfCommands())
        self.scene.AddActorToGroup(self.id, self.scene.FindByName("TickRegistry"))
        latest_command = self.PlaceGraphics(latest_command)

    @delegater.RegisterCommand(sc.InputCommand.TICK)
    def HandleTick(self, args):
        duration_seconds = args.duration / 1000.0
        self.pose.location += self.velocity * duration_seconds
        self.PlaceGraphics(self.scene.FrontOfCommands())

    def PlaceGraphics(self, latest_command):
        return self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.graphics_id),
            sc.PlaceComponent(
                "Sphere",
                self.pose))

    def LoadGraphicsResources(self, latest_command):
        self.graphics_id = self.scene.AddAndConstructGraphicsObject().id
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.graphics_id),
            sc.CreateGraphicsObject(
                "basic",
                sc.VectorEntitySpecification((
                    sc.EntitySpecification("Shell")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier("sphere.obj"),
                        sc.OutputFormat(
                            sc.ModelModifier(
                                sc.ArrayInt3((0, 1, 2)),
                                sc.ArrayFloat3((1, 1, 1)),
                                sc.ArrayBool2((False, True))),
                            sc.VertexType.normal,
                            False)))
                    .SetShaders(sc.ShaderDetails(
                        sc.VectorShaderIdentifier((
                            sc.ShaderIdentifier("vs_location_apply_mvp.cso", sc.ShaderStage.Vertex(), sc.VertexType.location),
                            sc.ShaderIdentifier("ps_solidcolor.cso", sc.ShaderStage.Pixel())))))
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat(self.color),)))
                    .SetComponent("Sphere"),)),
                sc.VectorComponentInfo((sc.ComponentInfo("", "Sphere"),))))
        return latest_command