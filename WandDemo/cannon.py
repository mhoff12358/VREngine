import scene_system as sc
import draggable_graphics, draggable_object, path_draggable_object, path, drag_direction_graphics, shell, cannon_ball
import math, enum, functools, copy

class CoverStatus(enum.Enum):
    OPEN = 0
    MIXED = 1
    CLOSED = 2

class Cannon(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    def __init__(self, starting_pose: sc.Pose, size: float):
        super().__init__()
        self.cannon_pose = copy.copy(starting_pose)
        self.cannon_pose.scale = sc.Scale(size)
        self.pitch = 0
        self.yaw = 0
        self.cover_position = sc.Pose(sc.Location(-0.62794, 0.78524, 0))
        self.cover_end_position = sc.Pose(sc.Location(-0.51281, 1.4757, 0))
        self.rotation_start = sc.Location(0, -0.4, 0)
        self.firing_pose = sc.Pose(sc.Location(0, 2, 0))
        self.cover_drag = None # type: path_draggable_object.PathDraggableObject
        self.draggable_cover = None # type: draggable_graphics.DraggableGraphics
        self.cannon_rotate = None # type: draggable_object.DraggableObject
        self.cannon_id = None # type: sc.ActorId
        self.cover_status = CoverStatus.CLOSED
        self.loading_collision_center = sc.Pose(sc.Location(-0.187, 0.71171, 0))
        self.loading_collision = sc.CollisionShape(self.loading_collision_center, 0)
        self.loaded_shell_id = None

    def UpdatePitch(self, pitch_delta):
        self.pitch += pitch_delta
        self.UpdatedRotation()

    def UpdateYaw(self, yaw_delta):
        self.yaw += yaw_delta
        self.UpdatedRotation()

    def UpdatedRotation(self):
        self.cannon_pose.orientation = sc.Quaternion.RotationAboutAxis(sc.AxisID.y, self.yaw) * sc.Quaternion.RotationAboutAxis(sc.AxisID.x, self.pitch)
        self.UpdateCannonPose()

    def IsLoaded(self):
        return self.loaded_shell_id is not None

    def Fire(self):
        if self.IsLoaded() and self.cover_status == CoverStatus.CLOSED:
            shell_attr_res = self.scene.AskQuery(sc.Target(self.loaded_shell_id), sc.QueryArgs(shell.ShellQueries.GET_ATTRIBUTES))
            loaded_shell_attributes = shell_attr_res.shell_attributes
            print(loaded_shell_attributes.power, loaded_shell_attributes.is_flare)
            self.scene.MakeCommandAfter(
                self.scene.FrontOfCommands(),
                sc.Target(self.loaded_shell_id),
                shell.FireShell())
            self.scene.MakeCommandAfter(
                self.scene.FrontOfCommands(),
                sc.Target(self.loaded_shell_id),
                shell.UpdateLoadPose(self.GetEjectedPose()))
            self.loaded_shell_id = None
            firing_pose = self.GetFiringPose()
            self.scene.AddActor(cannon_ball.CannonBall(
                loaded_shell_attributes,
                firing_pose,
                sc.Location(0, 1, 0).Rotate(firing_pose.orientation)))

    def TryLoadShell(self, shell_id) -> bool:
        if self.cover_status != CoverStatus.OPEN or self.IsLoaded():
            return False
        shell_attr_res = self.scene.AskQuery(sc.Target(shell_id), sc.QueryArgs(shell.ShellQueries.GET_ATTRIBUTES))
        loaded_shell_attributes = shell_attr_res.shell_attributes
        if loaded_shell_attributes.spent:
            return False
        self.loaded_shell_id = shell_id
        print("Loaded: ", loaded_shell_attributes.power, loaded_shell_attributes.is_flare)
        self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.loaded_shell_id),
            shell.LoadShell(self.GetLoadingPose()))
        return True

    def GetLoadingCollision(self):
        return self.loading_collision

    def SetCoverStatus(self, cover_status: CoverStatus):
        if self.cover_status != cover_status:
            print(cover_status.name)
        self.cover_status = cover_status

    def CoverDragged(self, global_pose: sc.Pose, relative_pose: sc.Pose, path_sample: path.NearestPoint, **kwargs):
        if path_sample.sample == 0:
            self.SetCoverStatus(CoverStatus.CLOSED)
        elif path_sample.sample == 1:
            self.SetCoverStatus(CoverStatus.OPEN)
        else:
            self.SetCoverStatus(CoverStatus.MIXED)

    def GetLoadingPose(self):
        return self.loading_collision_center.ApplyAfter(self.cannon_pose).WithoutScale()

    def GetEjectedPose(self):
        ejected_pose = copy.copy(self.loading_collision_center)
        ejected_pose.location += sc.Location(-1, 0, 0)
        return ejected_pose.ApplyAfter(self.cannon_pose).WithoutScale()

    def GetFiringPose(self):
        return self.firing_pose.ApplyAfter(self.cannon_pose).WithoutScale()

    def UpdateCannonPose(self):
        latest_command = self.scene.FrontOfCommands()
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.cannon_id),
            sc.PlaceComponent("Whole", self.cannon_pose))
        self.cover_drag.SetOffsetPose(self.cannon_pose)
        self.loading_collision.SetPose(self.GetLoadingPose())
        if self.loaded_shell_id is not None:
            self.scene.MakeCommandAfter(
                self.scene.FrontOfCommands(),
                sc.Target(self.loaded_shell_id),
                shell.UpdateLoadPose(self.GetLoadingPose()))

    @delegater.RegisterCommand(sc.HeadsetInterfaceCommand.LISTEN_TOUCHPAD_DRAG)
    def HandleTouchpadDrag(self, args):
        self.Fire()

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.scene.FindByName("HeadsetInterface")),
            sc.HeadsetListenerRegistration(
                True, self.id, sc.HeadsetListenerId.TOUCHPAD_DRAG))

        latest_command = self.scene.FrontOfCommands()
        latest_command = self.LoadGraphicsObjects(latest_command)
        # The draggable object that controls where the cover can be slid.
        self.cover_drag = path_draggable_object.PathDraggableObject(
            paths = path.Line(p0 = self.cover_position.location, p1 = self.cover_end_position.location, tube_radius = 0.1).AsPath(),
            draw_path = False,
            draw_ball = False,
            pose_updated_callback = functools.partial(self.CoverDragged))
        (_, latest_command) = self.scene.AddActorAfterReturnInitialize(self.cover_drag, latest_command)
        # Enforces that when the cover is dragged the model is updated.
        self.draggable_cover = draggable_graphics.DraggableGraphics(
            scene = self.scene,
            draggable_actor = self.cover_drag,
            graphics_id = self.cannon_id,
            graphics_component = "Cover",
            graphics_pose = sc.Pose(),
            delta_pose = self.cover_position)


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