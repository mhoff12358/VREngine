import scene_system as sc
import collections, copy, typing

class DraggableObject(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    def __init__(
        self,
        collision_shapes: typing.Iterable[typing.Tuple[sc.CollisionShape, sc.Pose]],
        starting_pose: sc.Pose = sc.Pose(),
        offset_pose: sc.Pose = sc.Pose(),
        preface_pose: sc.Pose = sc.Pose(),
        draw_ball: bool = False,
        pose_updated_callback: typing.Callable[[sc.Pose, sc.Pose], None] = None,
        pose_updated_callbacks: typing.Iterator[typing.Callable[[sc.Pose, sc.Pose], None]] = ()):
        super().__init__()

        self.stored_pose = copy.copy(starting_pose)
        self.current_pose = copy.copy(self.stored_pose)
        self.offset_pose = copy.copy(offset_pose)
        self.preface_pose = copy.copy(preface_pose)
        self.collision_shapes, self.collision_shape_offsets = zip(*collision_shapes)

        self.grab_pose = sc.Pose()

        self.pose_updated_callbacks = list(pose_updated_callbacks)
        if pose_updated_callback is not None:
            self.pose_updated_callbacks.append(pose_updated_callback)

        self.collision_sphere_id = draw_ball

    def GetPoseWithOffset(self):
        return self.offset_pose.ApplyAfter(self.current_pose)

    def AddPoseUpdatedCallback(self, callback):
        self.pose_updated_callbacks.append(callback)

    def ReposeCollisionShapes(self, latest_command):
        for i in range(len(self.collision_shape_offsets)):
            latest_command = self.scene.MakeCommandAfter(
                latest_command,
                sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
                sc.ReposeGrabbableObject(self.id, self.collision_shape_offsets[i].ApplyAfter(self.GetPoseWithOffset())))
        return latest_command

    def SetOffsetPose(self, offset_pose):
        self.current_pose = self.offset_pose.ApplyAfter(self.current_pose).Delta(self.offset_pose).ApplyAfter(offset_pose).UnapplyAfter(offset_pose)
        self.offset_pose = copy.copy(offset_pose)
        self.ReposeCollisionShapes(self.scene.FrontOfCommands())
        self.PlaceCollisionSphere(self.scene.FrontOfCommands())
        self.MakePoseUpdatedCallbacks()

    def ProposePose(self, proposed_pose):
        return (proposed_pose, None)

    def MakeCollisionSphere(self):
        self.collision_sphere_id = self.scene.AddAndConstructGraphicsObject().id
        latest_command = self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.collision_sphere_id),
            sc.CreateGraphicsObject(
                "basic",
                sc.VectorEntitySpecification(
                    (sc.EntitySpecification("sphere") .SetModel(
                        sc.ModelDetails(
                            sc.ModelIdentifier("sphere.obj|Sphere"))) .SetShaders(
                        sc.ShaderDetails(
                            sc.VectorShaderIdentifier(
                                (sc.ShaderIdentifier(
                                    "vs_location_apply_mvp.cso",
                                    sc.ShaderStage.Vertex(),
                                    sc.VertexType.location),
                                 sc.ShaderIdentifier(
                                    "ps_solidcolor.cso",
                                    sc.ShaderStage.Pixel()),
                                 )))) .SetShaderSettingsValue(
                        sc.ShaderSettingsValue(
                            (sc.VectorFloat(
                                (0.5,
                                 0,
                                 0)),
                             ))) .SetComponent("Sphere"),
                     )),
                sc.VectorComponentInfo(
                    (sc.ComponentInfo(
                        "",
                        "Sphere"),
                     ))))
        return latest_command

    def PlaceCollisionSphere(self, latest_command):
        if self.collision_sphere_id is not None:
            draw_pose = self.collision_shape_offsets[0].ApplyAfter(self.GetPoseWithOffset())
            self.scene.MakeCommandAfter(
                latest_command,
                sc.Target(self.collision_sphere_id),
                sc.PlaceComponent(
                    "Sphere",
                    sc.Pose(draw_pose.location, draw_pose.orientation, sc.Scale(0.075))))

    default_pose_updated_extra_response = {}
    def MakePoseUpdatedCallbacks(self, extra_response = None):
        if extra_response is None:
            extra_response = self.default_pose_updated_extra_response
        for callback in self.pose_updated_callbacks:
            callback(self.current_pose, self.offset_pose.ApplyAfter(self.current_pose).Delta(self.offset_pose), **extra_response)

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddToScene(self, args):
        latest_command = self.scene.FrontOfCommands()
        latest_command = self.scene.MakeCommandAfter(
            latest_command,
            sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
            sc.AddGrabbableObject(
                self.id,
                sc.VectorCollisionShape(self.collision_shapes)))
        latest_command = self.ReposeCollisionShapes(latest_command)
        self.collision_shapes = None
        if self.collision_sphere_id:
            latest_command = self.MakeCollisionSphere()
            self.PlaceCollisionSphere(latest_command)
        else:
            self.collision_sphere_id = None
        self.MakePoseUpdatedCallbacks()

    def SetNewPose(self, new_pose):
        (responded_pose, extra_response) = self.ProposePose(new_pose)
        if responded_pose is not None:
            self.current_pose = responded_pose
            self.MakePoseUpdatedCallbacks(extra_response)
            if self.collision_sphere_id is not None:
                self.PlaceCollisionSphere(self.scene.FrontOfCommands())

    @delegater.RegisterCommand(sc.HeadsetInterfaceCommand.LISTEN_CONTROLLER_MOVEMENT)
    def HandleControllerMovementWhileGrabbed(self, args):
        self.SetNewPose(self.controller_pose_delta.ApplyAfter(args.position))

    @delegater.RegisterCommand(sc.GrabbableObjectCommand.OBJECT_GRABBED)
    def HandleGrabbed(self, args):
        if args.held:
            self.controller_pose_delta = self.current_pose.Delta(args.position)
            self.scene.MakeCommandAfter(
                self.scene.FrontOfCommands(),
                sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
                sc.EnDisableGrabbableObject(self.id, False))
        else:
            self.stored_pose = self.current_pose
            self.ReposeCollisionShapes(self.scene.FrontOfCommands())
            self.scene.MakeCommandAfter(
                self.scene.FrontOfCommands(),
				sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
				sc.EnDisableGrabbableObject(self.id, True))
