import scene_system as sc
import draggable_graphics, draggable_object, path_draggable_object, path
import math


class Cannon(sc.DelegatingActor):
    command_delegation = sc.DelegatingActor.GetDefaultDelegation()
    delegater = sc.delegate_for_command(command_delegation)

    @delegater(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.scene.FindByName("HeadsetInterface")),
            sc.HeadsetListenerRegistration(
                True, self.id, sc.HeadsetListenerId.TOUCHPAD_DRAG))

    @delegater(sc.HeadsetInterfaceCommand.LISTEN_TOUCHPAD_DRAG)
    def HandleTouchpadDrag(self, args):
        self.drag_circle.SetOffsetPose(sc.Pose(sc.Location(2*args.position.x, 2*args.position.y, 0)))

    def __init__(self,
                 scene: sc.Scene):
        super().__init__()

        self.cannon_pose = sc.Pose(sc.Location(0, 1, 0), sc.Quaternion.RotationAboutAxis(sc.AxisID.y, 3.14/4))
        self.cover_position = sc.Pose(sc.Location(-0.43, 0.75, 0))

        self.scene = scene
        self.drag_obj = draggable_object.DraggableObject(
            collision_shapes = ((sc.CollisionShape(sc.Pose(), 0.1), self.cover_position),),
            starting_pose = sc.Pose(sc.Location(0, 0, 0)),
            offset_pose = self.cannon_pose,
            draw_ball = True)
        scene.AddActor(self.drag_obj)
        latest_command = scene.FrontOfCommands()
        square_id = scene.AddAndConstructGraphicsObject().id
        latest_command = scene.MakeCommandAfter(
            latest_command,
            sc.Target(square_id),
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
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(square_id),
            sc.PlaceComponent("Whole", self.cannon_pose))
        drag_graph = draggable_graphics.DraggableGraphics(
            scene = scene,
            draggable_actor = self.drag_obj,
            graphics_id = square_id,
            graphics_component = "Cover",
            graphics_pose = sc.Pose(),
            parent_pose = self.cannon_pose)


        # Makes the circular path
        num_samples = 10
        theta_samples = tuple(
            math.pi * 2 * i / num_samples for i in range(
                num_samples + 1))
        lines = tuple(
            path.Line(
                sc.Location(
                    math.cos(
                        theta_samples[i]),
                    2,
                    math.sin(
                        theta_samples[i])),
                sc.Location(
                    math.cos(
                        theta_samples[
                            i + 1]),
                    2,
                    math.sin(
                        theta_samples[
                            i + 1])),
                0.1) for i in range(num_samples))
        self.drag_circle = path_draggable_object.PathDraggableObject(
            path.Path(lines, circular=True), path_sample_rate=1)
        self.scene.AddActor(self.drag_circle)

