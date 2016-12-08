import scene_system as sc
import draggable_graphics, draggable_object, path_draggable_object, path, drag_direction_graphics, shell
import math, enum, functools, copy

class CannonCommands(sc.CommandRegistration):
    AIM_CANNON = None

class AimCannon(sc.CommandArgs):
    def __init__(self, orientation: sc.Quaternion):
        super().__init__(int(CannonCommands.AIM_CANNON))
        self.aim = orientation

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
        self.cover_position = sc.Pose(sc.Location(-0.43, 0.75, 0))
        self.cover_end_position = sc.Pose(sc.Location(-0.3, 1.536, 0))
        self.rotation_start = sc.Location(0, -0.4, 0)
        self.cover_drag = None # type: path_draggable_object.PathDraggableObject
        self.draggable_cover = None # type: draggable_graphics.DraggableGraphics
        self.cannon_rotate = None # type: draggable_object.DraggableObject
        self.cannon_id = None # type: sc.ActorId
        self.cover_status = CoverStatus.CLOSED
        self.loading_collision_center = sc.Pose(sc.Location(-0.187, 0.71171, 0))
        self.loading_collision = sc.CollisionShape(self.loading_collision_center, 0)
        self.loaded_shell_attributes = None

    def IsLoaded(self):
        return self.loaded_shell_attributes is not None

    def Fire(self):
        print(self.loaded_shell_attributes.power, self.loaded_shell_attributes.is_flare)
        self.loaded_shell_attributes = None

    def TryLoadShell(self, shell_id) -> bool:
        if self.cover_status != CoverStatus.OPEN or self.IsLoaded():
            return False
        import pdb; pdb.set_trace()
        query_args = sc.QueryArgs(int(shell.ShellQueries.GET_ATTRIBUTES))
        target = sc.Target(shell_id)
        shell_attr_res =self.scene.AskQuery(target, query_args)
        self.loaded_shell_attributes = shell_attr_res.shell_attributes
        print("Loaded: ", loaded_shell_attributes.power, loaded_shell_attributes.is_flare)
        return True

    def GetLoadingCollision(self):
        return self.loading_collision

    def SetCoverStatus(self, cover_status: CoverStatus):
        if self.cover_status != cover_status:
            print(cover_status.name)
        self.cover_status = cover_status

    def CannonRotationObjectDragged(self, global_pose: sc.Pose, relative_pose: sc.Pose, **kwargs):
        self.cannon_pose.orientation = sc.Quaternion.RotationBetweenLocations(self.rotation_start, relative_pose.location, 1)
        self.UpdateCannonPose()

    def CoverDragged(self, global_pose: sc.Pose, relative_pose: sc.Pose, path_sample: path.NearestPoint, **kwargs):
        if path_sample.sample == 0:
            self.SetCoverStatus(CoverStatus.CLOSED)
        elif path_sample.sample == 1:
            self.SetCoverStatus(CoverStatus.OPEN)
        else:
            self.SetCoverStatus(CoverStatus.MIXED)

    @delegater.RegisterCommand(CannonCommands.AIM_CANNON)
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
        self.loading_collision.SetPose(self.loading_collision_center.ApplyAfter(self.cannon_pose))

    @delegater.RegisterCommand(sc.HeadsetInterfaceCommand.LISTEN_TOUCHPAD_DRAG)
    def HandleTouchpadDrag(self, args):
        if self.IsLoaded() and self.cover_status == CoverStatus.CLOSED:
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
            paths = path.Line(p0 = self.cover_position.location, p1 = self.cover_end_position.location, radius = 0.1).AsPath(),
            path_sample_rate = 1,
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
        # Allows the user to rotate the cannon manually.
        self.cannon_rotate = draggable_object.DraggableObject(
            collision_shapes = ((sc.CollisionShape(sc.Pose(), 0.1), sc.Pose()),),
            starting_pose = sc.Pose(self.rotation_start),
            draw_ball = True,
            offset_pose = self.cannon_pose,
            pose_updated_callback = functools.partial(self.CannonRotationObjectDragged))
        (_, latest_command) = self.scene.AddActorAfterReturnInitialize(self.cannon_rotate, latest_command)


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