import scene_system as sc
import draggable_graphics, draggable_object, path, drag_direction_graphics
import math, functools, typing, copy

class ShellAttributes(object):
    def __init__(self, power, is_flare = False):
        self.power = power
        self.is_flare = is_flare

class ShellQueries(sc.QueryRegistration):
    GET_ATTRIBUTES = None

class RespondShellAttributes(sc.QueryResult):
    def __init__(self, shell_attributes: ShellAttributes):
        super().__init__(int(ShellQueries.GET_ATTRIBUTES))
        self.shell_attributes = shell_attributes

class Shell(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    def __init__(self, shell_attributes: ShellAttributes, starting_pose: sc.Pose = sc.Pose(), size: float = 1, reposed_callback = None):
        super().__init__()
        self.shell_attributes = shell_attributes
        self.graphics_id = None # type : sc.ActorId
        self.draggable_shell = None # type : draggable_object.DraggableObject
        self.shell_wrapper = None # type : draggable_graphics.DraggableGraphics
        self.shell_pose = sc.Pose()
        self.loading_collision = sc.CollisionShape(sc.Pose(), 0.1)
        self.reposed_callback = reposed_callback

        starting_pose = copy.copy(starting_pose)
        starting_pose.scale = sc.Scale(size)
        self.SetShellPose(starting_pose)

    def SetShellPose(self, shell_pose):
        self.shell_pose = shell_pose
        self.loading_collision.SetPose(self.shell_pose)
        if self.reposed_callback is not None:
            self.reposed_callback(self)

    def ShellDragged(self, global_pose, relative_pose):
        self.SetShellPose(relative_pose)

    def GetLoadingCollision(self):
        return self.loading_collision

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        latest_command = self.scene.FrontOfCommands()
        latest_command = self.LoadGraphicsResources(latest_command)

        self.draggable_shell = draggable_object.DraggableObject(
            collision_shapes = ((sc.CollisionShape(sc.Pose(), 0.1), sc.Pose()),),
            starting_pose = self.shell_pose,
            pose_updated_callback = functools.partial(self.ShellDragged))
        self.scene.AddActorAfter(self.draggable_shell, latest_command)
        self.shell_wrapper = draggable_graphics.DraggableGraphics(
            scene = self.scene,
            draggable_actor = self.draggable_shell,
            graphics_id = self.graphics_id,
            graphics_component = "Cylinder",
            graphics_pose = sc.Pose(sc.Scale(0.1, 0.2, 0.1)))

    @delegater.RegisterQuery(int(ShellQueries.GET_ATTRIBUTES))
    def HandleGetAttributes(self, args):
        return sc.Temp(int(ShellQueries.GET_ATTRIBUTES))
        #return RespondShellAttributes(copy.copy(self.shell_attributes))

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
                        sc.ModelIdentifier("cylinder.obj"),
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
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((0.75, 0.75, 0.75)),)))
                    .SetComponent("Cylinder"),)),
                sc.VectorComponentInfo((sc.ComponentInfo("", "Cylinder"),))))
        return latest_command