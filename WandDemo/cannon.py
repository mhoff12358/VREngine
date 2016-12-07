import scene_system as sc
import draggable_graphics, draggable_object, path_draggable_object, path, drag_direction_graphics
import math

class CannonCommands(sc.CommandRegistration):
    AIM_CANNON = None

class AimCannon(sc.CommandArgs):
    def __init__(self, orientation: sc.Quaternion):
        super().__init__(CannonCommands.AIM_CANNON)
        self.aim = orientation

class Cannon(sc.DelegatingActor):
    command_delegation = sc.DelegatingActor.GetDefaultDelegation()
    delegater = sc.delegate_for_command(command_delegation)

    def CannonRotationObjectDragged(self, global_pose, relative_pose):
        self.cannon_pose.orientation = sc.Quaternion.RotationBetweenLocations(self.rotation_start, relative_pose.location, 1)
        self.UpdateCannonPose()

    @delegater(CannonCommands.AIM_CANNON)
    def HandleAimCannon(self, args: AimCannon):
        self.cannon_pose.orientation = args.aim
        self.UpdateCannonPose()

    def UpdateCannonPose(self):
        latest_command = self.scene.FrontOfCommands()
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.cannon_id),
            sc.PlaceComponent("Whole", self.cannon_pose))
        self.cover_drag.SetOffsetPose(self.cannon_pose)

    @delegater(sc.HeadsetInterfaceCommand.LISTEN_TOUCHPAD_DRAG)
    def HandleTouchpadDrag(self, args):
        pass

    @delegater(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.scene.FindByName("HeadsetInterface")),
            sc.HeadsetListenerRegistration(
                True, self.id, sc.HeadsetListenerId.TOUCHPAD_DRAG))
        self.cannon_pose = sc.Pose(sc.Location(0, 1, 0), sc.Scale(0.5))
        self.cover_position = sc.Pose(sc.Location(-0.43, 0.75, 0))
        self.cover_end_position = sc.Pose(sc.Location(-0.3, 1.536, 0))
        self.rotation_start = sc.Location(0, -0.4, 0)

        latest_command = self.scene.FrontOfCommands()
        latest_command = self.LoadGraphicsObjects(latest_command)
        self.cover_drag = path_draggable_object.PathDraggableObject(
            path.Line(p0 = self.cover_position.location, p1 = self.cover_end_position.location, radius = 0.1).AsPath(), None, 1)
        (_, latest_command) = self.scene.AddActorAfterReturnInitialize(self.cover_drag, latest_command)
        self.draggable_cover = draggable_graphics.DraggableGraphics(
            scene = self.scene,
            draggable_actor = self.cover_drag,
            graphics_id = self.cannon_id,
            graphics_component = "Cover",
            graphics_pose = sc.Pose(),
            delta_pose = self.cover_position)
        self.cannon_rotate = draggable_object.DraggableObject(
            collision_shapes = ((sc.CollisionShape(sc.Pose(), 0.1), sc.Pose()),),
            starting_pose = sc.Pose(self.rotation_start),
            draw_ball = True,
            offset_pose = self.cannon_pose,
            pose_updated_callback = lambda *args, **kwargs: self.CannonRotationObjectDragged(*args, **kwargs))
        (_, latest_command) = self.scene.AddActorAfterReturnInitialize(self.cannon_rotate, latest_command)
        #self.cover_drag = draggable_object.DraggableObject(
        #    collision_shapes = ((sc.CollisionShape(sc.Pose(), 0.1), self.cover_position),),
        #    starting_pose = sc.Pose(sc.Location(0, 0, 0)),
        #    draw_ball = True)


    def LoadGraphicsObjects(self, latest_command):
        self.cannon_id = self.scene.AddAndConstructGraphicsObject().id
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.cannon_id),
            sc.CreateGraphicsObject(
                "basic",
                    sc.VectorEntitySpecification((
                        sc.EntitySpecification("Cannon")
                        .SetModel(sc.ModelDetails(
                            sc.ModelIdentifier("cannon.obj|Cannon"),
                            sc.OutputFormat(
                                sc.ModelModifier(
                                    sc.ArrayInt3((0, 1, 2)),
                                    sc.ArrayFloat3((1, 1, 1)),
                                    sc.ArrayBool2((False, True))),
                                sc.VertexType.texture,
                                False)))
                        .SetShaders(sc.ShaderDetails(
                            sc.VectorShaderIdentifier((
                                sc.ShaderIdentifier("vs_texture_apply_mvp.cso", sc.ShaderStage.Vertex(), sc.VertexType.texture),
                                sc.ShaderIdentifier("ps_textured.cso", sc.ShaderStage.Pixel())))))
                        .SetShaderSettingsValue(sc.ShaderSettingsValue(tuple()))
                        .SetTextures(sc.VectorIndividualTextureDetails((sc.IndividualTextureDetails("cannon2.png", sc.ShaderStages.All(), 0, 0),)))
                        .SetComponent("Cannon"),
                        sc.EntitySpecification("Cover")
                        .SetModel(sc.ModelDetails(
                            sc.ModelIdentifier("cannon.obj|Cover"),
                            sc.OutputFormat(
                                sc.ModelModifier(
                                    sc.ArrayInt3((0, 1, 2)),
                                    sc.ArrayFloat3((1, 1, 1)),
                                    sc.ArrayBool2((False, True))),
                                sc.VertexType.texture,
                                False)))
                        .SetShaders(sc.ShaderDetails(
                            sc.VectorShaderIdentifier((
                                sc.ShaderIdentifier("vs_texture_apply_mvp.cso", sc.ShaderStage.Vertex(), sc.VertexType.texture),
                                sc.ShaderIdentifier("ps_textured.cso", sc.ShaderStage.Pixel())))))
                        .SetShaderSettingsValue(sc.ShaderSettingsValue(tuple()))
                        .SetTextures(sc.VectorIndividualTextureDetails((sc.IndividualTextureDetails("cannon2.png", sc.ShaderStages.All(), 0, 0),)))
                        .SetComponent("Cover"),)),
                    sc.VectorComponentInfo((sc.ComponentInfo("", "Whole"),
                                            sc.ComponentInfo("Whole", "Cover"),
                                            sc.ComponentInfo("Whole", "Cannon")))))
        return latest_command