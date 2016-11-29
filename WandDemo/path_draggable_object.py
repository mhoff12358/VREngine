import scene_system as sc
import draggable_object
from itertools import *

class PathDraggableObject(draggable_object.DraggableObject):
    command_delegation = draggable_object.DraggableObject.GetDefaultDelegation()
    delegater = sc.delegate_for_command(command_delegation)

    def __init__(self, paths, path_sample_rates=None, path_sample_rate=5):
        self.radius = 0.1

        self.paths = paths
        self.path_vertices = self.paths.CreateSamples(
            path_sample_rates, path_sample_rate)

        super().__init__(((sc.CollisionShape(sc.Pose(), self.radius), sc.Pose()),),
                         sc.Pose(self.paths.At(0)), draw_ball = True)

    def SetOffsetPose(self, offset_pose):
        super().SetOffsetPose(offset_pose)
        self.PlacePath(self.scene.FrontOfCommands())

    def PlacePath(self, latest_command):
        return self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.path_id),
            sc.PlaceComponent(
                "path",
                self.offset_pose))

    @delegater(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddToScene(self, args):
        super().HandleAddToScene(args)
        latest_command = self.scene.FrontOfCommands()

        self.graphics_resources = sc.GraphicsResources.GetGraphicsResources(
            self.scene)

        self.path_id = self.scene.AddAndConstructGraphicsObject().id
        path_model_generator = sc.ModelGenerator(
            sc.VertexType.location, sc.D3DTopology.LINESTRIP_ADJ)
        path_model_generator.AddVertexBatch(
            sc.Vertices(
                sc.VertexType.location, sc.VectorArrayFloat3(
                    chain(
                        self.path_vertices[-2:-1]
                        if self.paths.circular else self.path_vertices[: 1],
												self.path_vertices,
                        self.path_vertices[-1:]))))
        path_model_generator.SetParts(
            sc.MapStringToModelSlice({"path": sc.ModelSlice(6, 0)}))
        path_model_generator.Finalize(
            self.graphics_resources.GetDeviceInterface(),
            self.graphics_resources.GetEntityHandler(),
            sc.ModelStorageDescription(False, True, False))
        self.path_model_name = sc.ResourceIdentifier.GetNewModelName(
            "draggable_path")
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.path_id),
            sc.CreateGraphicsObject(
                "basic",
                sc.VectorEntitySpecification((
                    sc.EntitySpecification("path")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier(self.path_model_name),
                        path_model_generator))
                    .SetShaders(sc.ShaderDetails(
                                sc.VectorShaderIdentifier((
                                    sc.ShaderIdentifier("vs_location_passthrough.cso", sc.ShaderStage.Vertex(), sc.VertexType.location),
                                    sc.ShaderIdentifier("gs_thick_line.cso", sc.ShaderStage.Geometry()),
                                    sc.ShaderIdentifier("ps_color_width.cso", sc.ShaderStage.Pixel()),
                                ))))
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((
                                            sc.VectorFloat((0.5, 0.5, 0.5, 1)),
                                            sc.VectorFloat((0.05,)))))
                    .SetTextures(sc.VectorIndividualTextureDetails((sc.IndividualTextureDetails("terrain.png", sc.ShaderStages.All(), 0, 0),)))
                    .SetComponent("path"),)),
                sc.VectorComponentInfo((sc.ComponentInfo("", "path"),))))
        latest_command = self.PlacePath(latest_command)

    def ProposePose(self, proposed_pose):
        nearest = self.paths.FindNearest(
            proposed_pose.location, return_distance_squared=True)
        if not nearest.found:
            return None

        proposed_pose.location = self.paths.At(nearest.sample)
        return correct_pose
