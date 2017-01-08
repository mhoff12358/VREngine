import scene_system as sc
import shader_helper
import math

class PanelSet(object):
    def __init__(self, scene, panel_size = 0.5, dimensions = (10, 10, 10), corner = sc.Location(-2.5, 0, -2.5)):
        self.scene = scene
        self.corner = corner
        self.panel_size = panel_size
        self.dimensions = dimensions
        self.CreatePanels()

    def CreatePanels(self):
        wall_sizes = (self.dimensions[1] * self.dimensions[2], self.dimensions[0] * self.dimensions[2], self.dimensions[0] * self.dimensions[1])
        num_panels = sum(wall_sizes) * 2

        self.graphics_ids = tuple(self.scene.AddAndConstructGraphicsObject().id for i in range(num_panels))

        for graphics_id in self.graphics_ids:
            self.scene.MakeCommandAfter(
                self.scene.BackOfNewCommands(),
                sc.Target(graphics_id),
                sc.CreateGraphicsObject(
                    "basic",
                    sc.VectorEntitySpecification((
                        sc.EntitySpecification("Panel")
                        .SetModel(sc.ModelDetails(
                            sc.ModelIdentifier("panel.obj"),
                            sc.OutputFormat(
                                sc.ModelModifier(
                                    sc.ArrayInt3((0, 1, 2)),
                                    sc.ArrayFloat3((1, 1, 1)),
                                    sc.ArrayBool2((False, True))),
                                sc.VertexType.all,
                                False)))
                        .SetShaders(shader_helper.ShaderHelper.Default(pixel_shader_name = "ps_textured", lighting = True))
                        .SetTextures(sc.VectorIndividualTextureDetails((sc.IndividualTextureDetails("panel.png", sc.ShaderStages.All(), 0, 0),)))
                        #.SetShaderSettingsValue(sc.ShaderSettingsValue((self.color.AsVector(),)))
                        .SetComponent("Panel"),)),
                    sc.VectorComponentInfo((sc.ComponentInfo("", "Panel"),))))


        orientations = {
            False: (
                sc.Quaternion.RotationAboutAxis(sc.AxisID.x, math.pi/2.0),
                sc.Quaternion.RotationAboutAxis(sc.AxisID.y, math.pi/2.0),
                sc.Quaternion.RotationAboutAxis(sc.AxisID.z, math.pi/2.0),
            ),
            True: (
                sc.Quaternion.RotationAboutAxis(sc.AxisID.x, -math.pi/2.0),
                sc.Quaternion.RotationAboutAxis(sc.AxisID.y, -math.pi/2.0),
                sc.Quaternion.RotationAboutAxis(sc.AxisID.z, -math.pi/2.0),
            )
        }
        orientations = {
            False: (
                sc.Quaternion.RotationAboutAxis(sc.AxisID.z, -math.pi/2.0),
                sc.Quaternion.Identity(),
                sc.Quaternion.RotationAboutAxis(sc.AxisID.x, math.pi/2.0),
            ),
            True: (
                sc.Quaternion.RotationAboutAxis(sc.AxisID.z, math.pi/2.0),
                sc.Quaternion.RotationAboutAxis(sc.AxisID.x, -math.pi),
                sc.Quaternion.RotationAboutAxis(sc.AxisID.x, -math.pi/2.0),
            )
        }

        already_placed_panels = 0
        for unused_dimension in range(3):
            for upside_down in (False, True):
                dimx = (unused_dimension + 1) % 3
                dimy = (unused_dimension + 2) % 3
                for x in range(self.dimensions[dimx]):
                    for y in range(self.dimensions[dimy]):
                        panel_num = already_placed_panels + y * self.dimensions[dimx] + x
                        location = [0, 0, 0]
                        location[unused_dimension] = self.dimensions[unused_dimension] * self.panel_size if upside_down else 0
                        location[dimx] = x * self.panel_size + self.panel_size * 0.5
                        location[dimy] = y * self.panel_size + self.panel_size * 0.5
                        self.scene.MakeCommandAfter(
                            self.scene.BackOfNewCommands(),
                            sc.Target(self.graphics_ids[panel_num]),
                            sc.PlaceComponent("Panel", sc.Pose(self.corner + sc.Location(*location), orientations[upside_down][unused_dimension], sc.Scale(self.panel_size * 0.5))))
                already_placed_panels += wall_sizes[unused_dimension]

