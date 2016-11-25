import scene_system as sc
import draggable_graphics, draggable_object, path_draggable_object


class Cannon(object):
    def __init__(self,
                 scene: sc.Scene):
        self.scene = scene
        drag_obj = draggable_object.DraggableObject(
            collision_shapes = ((sc.CollisionShape(sc.Pose(), 1), sc.Pose()),),
            starting_pose = sc.Pose(sc.Location(0, 0, 0)),
            offset_pose = sc.Pose())
            #offset_pose = sc.Pose(sc.Location(-.43, .75, 0)))
        scene.AddActor(drag_obj)
        latest_command = scene.FrontOfCommands()
        square_id = scene.AddAndConstructGraphicsObject().id
        square_pose = sc.Pose(sc.Location(0, 1, 0), sc.Scale(0.25))
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
        drag_graph = draggable_graphics.DraggableGraphics(
            scene = scene,
            draggable_actor = drag_obj,
            graphics_id = square_id,
            graphics_component = "Whole",
            graphics_pose = sc.Pose())