import scene_system as sc
import draggable_object, draggable_graphics, shader_helper
import math, functools, copy

class Lighter(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    shaft_color = (0.388, 0.149, 0)
    tip_color = (1, 0.5, 0)

    def __init__(self, starting_pose = sc.Pose(), reposed_callback = None):
        self.starting_pose = starting_pose
        self.tip_pose = sc.Pose(sc.Location(0, 0.75, 0))
        self.tip_collision = sc.CollisionShape(self.tip_pose, 0.1)
        self.handle_pose = sc.Pose(sc.Location(0, 0.1, 0))
        self.current_offset_pose = sc.Pose()
        self.reposed_callback = reposed_callback
        super().__init__()

    def Reposed(self, global_pose, relative_pose, **kwargs):
        self.current_offset_pose = copy.copy(relative_pose)
        self.tip_collision.SetPose(self.tip_pose.ApplyAfter(self.current_offset_pose))
        if self.reposed_callback is not None:
            self.reposed_callback(self)

    def GetTipCollision(self):
        return self.tip_collision

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        self.LoadGraphicsResources()
        self.draggable_handle = draggable_object.DraggableObject(
            collision_shapes = ((sc.CollisionShape(sc.Pose(), 0.1), self.handle_pose),),
            starting_pose = self.starting_pose,
            pose_updated_callback = functools.partial(self.Reposed))
        self.scene.AddActor(self.draggable_handle)
        self.starting_pose = None
        self.draggable_handle_graphics = draggable_graphics.DraggableGraphics(self.scene, self.draggable_handle, self.graphics_id, "Whole")

    def LoadGraphicsResources(self):
        self.graphics_id = self.scene.AddAndConstructGraphicsObject().id
        shader_details = shader_helper.ShaderHelper.Default(pixel_shader_name = "ps_solidcolor.cso")
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            sc.Target(self.graphics_id),
            sc.CreateGraphicsObject(
                "basic",
                sc.VectorEntitySpecification((
                    sc.EntitySpecification("Shaft")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier("lighter.obj|Shaft"),
                        sc.OutputFormat(
                            sc.ModelModifier(
                                sc.ArrayInt3((0, 1, 2)),
                                sc.ArrayFloat3((1, 1, 1)),
                                sc.ArrayBool2((False, True))),
                            sc.VertexType.all,
                            False)))
                    .SetShaders(shader_details)
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat(self.shaft_color),)))
                    .SetComponent("Whole"),
                    sc.EntitySpecification("Tip")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier("lighter.obj|Tip"),
                        sc.OutputFormat(
                            sc.ModelModifier(
                                sc.ArrayInt3((0, 1, 2)),
                                sc.ArrayFloat3((1, 1, 1)),
                                sc.ArrayBool2((False, True))),
                            sc.VertexType.all,
                            False)))
                    .SetShaders(shader_details)
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat(self.tip_color),)))
                    .SetComponent("Whole"),
                    )),
                sc.VectorComponentInfo((sc.ComponentInfo("", "Whole"),))))
