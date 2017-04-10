import scene_system as sc
import draggable_object, draggable_graphics, shader_helper, light
import math, functools, copy

class Lighter(sc.DelegatingActor[sc.Actor]):
    delegater = sc.Delegater(sc.DelegatingActor[sc.Actor])

    shaft_color = (0.388, 0.149, 0)
    tip_color = sc.Color(1, 0.5, 0, 3)

    def __init__(self, starting_pose = sc.Pose(), reposed_callback = None, light_system_name = ""):
        self.starting_pose = starting_pose
        self.tip_pose = sc.Pose(sc.Location(0, 0.75, 0))
        self.tip_collision = sc.CollisionShape(self.tip_pose, 0.1)
        self.handle_pose = sc.Pose(sc.Location(0, 0.1, 0))
        self.current_offset_pose = sc.Pose()
        self.reposed_callback = reposed_callback
        self.light_system_name = light_system_name
        super().__init__()

    def Reposed(self, global_pose, relative_pose, **kwargs):
        self.current_offset_pose = copy.copy(relative_pose)
        actual_tip_pose = self.tip_pose.ApplyAfter(self.current_offset_pose)
        self.tip_light.SetLocation(actual_tip_pose.location)
        self.tip_collision.SetPose(actual_tip_pose)
        if self.reposed_callback is not None:
            self.reposed_callback(self)

    def Grabbed(self, grabbed, hand_index, hand_pose):
        if grabbed:
            pass
            #self.scene.MakeCommandAfter(
            #    self.scene.FrontOfCommands(),
            #    sc.Command(
            #        sc.Target(self.scene.FindByName("HeadsetInterface")),
            #        sc.HapticPulse(hand_index, 1, 1)));
        else:
            pass

    def GetTipCollision(self):
        return self.tip_collision

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        self.LoadGraphicsResources()
        self.tip_light = light.PointLight(
            location = self.tip_pose.ApplyAfter(self.starting_pose).location,
            color = self.tip_color,
            light_system_name = self.light_system_name,
            number = 1,
            scene = self.scene)
        self.draggable_handle = draggable_object.DraggableObject(
            collision_shapes = ((sc.CollisionShape(sc.Pose(), 0.1), self.handle_pose),),
            starting_pose = self.starting_pose,
            pose_updated_callback = functools.partial(self.Reposed),
            grab_callback = functools.partial(self.Grabbed))
        self.scene.AddActor(self.draggable_handle)
        self.starting_pose = None
        self.draggable_handle_graphics = draggable_graphics.DraggableGraphics(self.scene, self.draggable_handle, self.graphics_id, "Whole")

    def LoadGraphicsResources(self):
        self.graphics_id = self.scene.AddAndConstructGraphicsObject().id
        shader_details_unlit = shader_helper.ShaderHelper.Default(pixel_shader_name = "ps_solidcolor", lighting = False)
        shader_details = shader_helper.ShaderHelper.Default(pixel_shader_name = "ps_solidcolor", lighting = True)
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
                    .SetShaders(shader_details_unlit)
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((self.tip_color.AsVector(),)))
                    .SetComponent("Whole"),
                    )),
                sc.VectorComponentInfo((sc.ComponentInfo("", "Whole"),))))
