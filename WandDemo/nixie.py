import scene_system as sc
import collections, copy, typing

class Nixie(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    def __init__(
        self,
        digit: int,
        pose: sc.Pose = sc.Pose()):
        super().__init__()

        self.pose = pose

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        self.graphics_id = self.scene.AddAndConstructGraphicsObject().id
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            sc.Target(self.graphics_id),
            sc.CreateGraphicsObject(
                "bloom_alpha",
                sc.VectorEntitySpecification((
                    sc.EntitySpecification("Square")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier("square_tex.obj"),
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
                            sc.ShaderIdentifier("ps_nixie.cso", sc.ShaderStage.Pixel())))))
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((1, 1, 1, 0.9)),)))
                    .SetTextures(
                        sc.VectorIndividualTextureDetails((
                            sc.IndividualTextureDetails("nixie9.png", sc.ShaderStages(sc.ShaderStage.Pixel()), 0, 0),
                            ))
                        )
                    .SetComponent("Square"),)),
                sc.VectorComponentInfo((sc.ComponentInfo("", "Square"),))))

        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            sc.Target(self.graphics_id),
            sc.PlaceComponent(
                "Square",
                self.pose))
