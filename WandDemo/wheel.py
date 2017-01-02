import scene_system as sc
import draggable_graphics, path_draggable_object, path, shader_helper
import copy, functools

class Wheel(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    def __init__(self, pose: sc.Pose, spun_callback = None, spun_callbacks = (), crank_height = 1, color = (0.5, 0, 0)):
        super().__init__()
        self.color = color
        self.pose = copy.copy(pose)
        self.crank_height = crank_height
        self.last_path_sample = 0
        self.spun_callbacks = list(spun_callbacks)
        if spun_callback is not None:
            self.spun_callbacks.append(spun_callback)

    def AddCallback(self, callback):
        self.spun_callbacks.append(callback)

    def Dragged(self, global_pose, relative_pose, path_sample, **kwargs):
        sample = path_sample.sample
        path_sample_delta = ((sample - self.last_path_sample + 0.5) % 1) - 0.5
        for callback in self.spun_callbacks:
            callback(path_sample_delta)
        self.last_path_sample = sample
        self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.graphics_id),
            sc.PlaceComponent(
                "Wheel",
                sc.Pose(relative_pose.orientation).ApplyAfter(self.pose)))

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        latest_command = self.LoadGraphicsResources(self.scene.FrontOfCommands())
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.graphics_id),
            sc.PlaceComponent(
                "Crank",
                sc.Pose(sc.Scale(1, self.crank_height, 1))))
        self.draggable_crank = path_draggable_object.PathDraggableObject(
            paths = path.CirclePath(
                center = sc.Location(0, self.crank_height, 0),
                up_vec = sc.Location(1, 0, 0),
                right_vec = sc.Location(0, 0, -1),
                radius = 1,
                num_samples = 20,
                tube_radius = 0.5
                ),
            draw_ball = False,
            draw_path = False,
            pose_updated_callback = functools.partial(self.Dragged),
            offset_pose = self.pose)
        self.scene.AddActorAfter(self.draggable_crank, latest_command)
        

    def LoadGraphicsResources(self, latest_command):
        self.graphics_id = self.scene.AddAndConstructGraphicsObject().id
        shader_details = shader_helper.ShaderHelper.Default(pixel_shader_name = "ps_solidcolor")
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.graphics_id),
            sc.CreateGraphicsObject(
                "basic",
                sc.VectorEntitySpecification((
                    sc.EntitySpecification("Wheel")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier("wheel.obj|Wheel"),
                        sc.OutputFormat(
                            sc.ModelModifier(
                                sc.ArrayInt3((0, 1, 2)),
                                sc.ArrayFloat3((1, 1, 1)),
                                sc.ArrayBool2((False, True))),
                            sc.VertexType.all,
                            False)))
                    .SetShaders(shader_details)
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat(self.color),)))
                    .SetComponent("Wheel"),
                    sc.EntitySpecification("Crank")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier("wheel.obj|Crank"),
                        sc.OutputFormat(
                            sc.ModelModifier(
                                sc.ArrayInt3((0, 1, 2)),
                                sc.ArrayFloat3((1, 1, 1)),
                                sc.ArrayBool2((False, True))),
                            sc.VertexType.all,
                            False)))
                    .SetShaders(shader_details)
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat(self.color),)))
                    .SetComponent("Crank"),
                    )),
                sc.VectorComponentInfo((sc.ComponentInfo("", "Wheel"),
                                        sc.ComponentInfo("Wheel", "Crank")))))
        return latest_command
