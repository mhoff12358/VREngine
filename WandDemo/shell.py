import scene_system as sc
import draggable_graphics, draggable_object, path, drag_direction_graphics, cannon
import math

class Shell(sc.DelegatingActor):
    command_delegation = sc.DelegatingActor.GetDefaultDelegation()
    delegater = sc.delegate_for_command(command_delegation)

    def __init__(self):
        super().__init__()
        self.shell_id = None # type : sc.ActorId
        self.draggable_shell = None # type : draggable_object.DraggableObject
        self.shell_wrapper = None # type : draggable_graphics.DraggableGraphics
        self.shell_pose = sc.Pose(sc.Location(1, 1, 0))

    @delegater(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        self.shell_id = self.scene.AddAndConstructGraphicsObject().id
        latest_command = self.scene.FrontOfCommands()
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.shell_id),
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
        self.draggable_shell = draggable_object.DraggableObject(
            collision_shapes = ((sc.CollisionShape(sc.Pose(), 0.1), sc.Pose()),),
            starting_pose = self.shell_pose,
            draw_ball = True)
        self.scene.AddActorAfter(self.draggable_shell, latest_command)
        self.shell_wrapper = draggable_graphics.DraggableGraphics(
            scene = self.scene,
            draggable_actor = self.draggable_shell,
            graphics_id = self.shell_id,
            graphics_component = "Cylinder",
            graphics_pose = sc.Pose(sc.Scale(0.1, 0.2, 0.1)))
