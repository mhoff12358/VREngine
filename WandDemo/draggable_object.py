import scene_system as sc
import collections, copy, typing

class DraggableObject(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    def __init__(
        self,
        collision_shapes: typing.Iterable[typing.Tuple[sc.CollisionShape, sc.Pose]],
        starting_pose: sc.Pose = sc.Pose(),
        offset_pose: sc.Pose = sc.Pose(),
        draw_ball: bool = False,
        pose_updated_callback: typing.Callable[[sc.Pose, sc.Pose], None] = None,
        pose_updated_callbacks: typing.Iterator[typing.Callable[[sc.Pose, sc.Pose], None]] = (),
        grab_callback: typing.Callable[[bool, int, sc.Pose], None] = None,
        grab_callbacks: typing.Iterator[typing.Callable[[bool, int, sc.Pose], None]] = ()):
        super().__init__()

        self.current_pose = copy.copy(starting_pose)
        self.offset_pose = copy.copy(offset_pose)
        self.collision_shapes, self.collision_shape_offsets = zip(*collision_shapes)

        self.grab_pose = sc.Pose()

        self.pose_updated_callbacks = list(pose_updated_callbacks)
        if pose_updated_callback is not None:
            self.pose_updated_callbacks.append(pose_updated_callback)

        self.grab_callbacks = list(grab_callbacks)
        if grab_callback is not None:
            self.grab_callbacks.append(grab_callback)
        self.grabbing_hand_index = None

        self.collision_sphere_id = draw_ball

    def GetPoseWithOffset(self):
        return self.offset_pose.ApplyAfter(self.current_pose)

    def AddPoseUpdatedCallback(self, callback):
        self.pose_updated_callbacks.append(callback)

    def ReposeCollisionShapes(self):
        for i in range(len(self.collision_shape_offsets)):
            self.scene.MakeCommandAfter(
                self.scene.BackOfNewCommands(),
                sc.Target(self.scene.FindByName("GrabbableObjectHandler")),
                sc.ReposeCollideableObject(self.id, self.collision_shape_offsets[i].ApplyAfter(self.GetPoseWithOffset())))

    def SetOffsetPose(self, offset_pose):
        self.current_pose = self.offset_pose.ApplyAfter(self.current_pose).Delta(self.offset_pose).ApplyAfter(offset_pose).UnapplyAfter(offset_pose)
        self.offset_pose = copy.copy(offset_pose)
        self.ReposeCollisionShapes()
        self.PlaceCollisionSphere()
        self.MakePoseUpdatedCallbacks()

    def ProposePose(self, proposed_pose):
        return (proposed_pose, None)

    def MakeCollisionSphere(self):
        self.collision_sphere_id = self.scene.AddAndConstructGraphicsObject().id
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
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

    def PlaceCollisionSphere(self):
        if self.collision_sphere_id is not None:
            draw_pose = self.collision_shape_offsets[0].ApplyAfter(self.GetPoseWithOffset())
            self.scene.MakeCommandAfter(
                self.scene.BackOfNewCommands(),
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
        self.grabbable_object_handler = sc.Target(self.scene.FindByName("GrabbableObjectHandler"))

        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            self.grabbable_object_handler,
            sc.AddCollideableObject(
                self.id,
                sc.VectorCollisionShape(self.collision_shapes)))
        self.ReposeCollisionShapes()
        self.collision_shapes = None
        if self.collision_sphere_id:
            self.MakeCollisionSphere()
            self.PlaceCollisionSphere()
        else:
            self.collision_sphere_id = None
        offset_pose = self.offset_pose
        self.offset_pose = sc.Pose()
        self.SetOffsetPose(offset_pose)

    def SetNewPose(self, new_pose):
        (responded_pose, extra_response) = self.ProposePose(new_pose)
        if responded_pose is not None:
            self.current_pose = responded_pose
            self.MakePoseUpdatedCallbacks(extra_response)
            if self.collision_sphere_id is not None:
                self.PlaceCollisionSphere()

    @delegater.RegisterCommand(sc.HeadsetInterfaceCommand.LISTEN_CONTROLLER_MOVEMENT)
    def HandleControllerMovementWhileGrabbed(self, args):
        if self.controller_pose_delta is not None:
            self.SetNewPose(self.controller_pose_delta.ApplyAfter(args.position))

    def GrabCallbacks(self, hand_index, hand_pose):
        self.grabbing_hand_index = hand_index
        for grab_callback in self.grab_callbacks:
            grab_callback(True, hand_index, hand_pose)

    def ReleaseCallbacks(self):
        for grab_callback in self.grab_callbacks:
            grab_callback(False, self.grabbing_hand_index, None)

    @delegater.RegisterCommand(sc.GrabbableObjectCommand.OBJECT_GRABBED)
    def HandleGrabbed(self, args):
        if args.held:
            self.controller_pose_delta = self.current_pose.Delta(args.position)
            self.scene.MakeCommandAfter(
                self.scene.BackOfNewCommands(),
				self.grabbable_object_handler,
                sc.EnDisableCollideableObject(self.id, False))
            self.GrabCallbacks(args.number, args.position)
        else:
            if self.controller_pose_delta is not None:
                self.controller_pose_delta = None
                self.ReposeCollisionShapes()
                self.scene.MakeCommandAfter(
                    self.scene.BackOfNewCommands(),
                    self.grabbable_object_handler,
                    sc.EnDisableCollideableObject(self.id, True))
                self.ReleaseCallbacks()

    def MoveToPose(self, new_pose):
        self.SetNewPose(new_pose)
        self.ReposeCollisionShapes()

    def DisableGrabbing(self):
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
			self.grabbable_object_handler,
            sc.DropGrabbableObject(self.id))
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
			self.grabbable_object_handler,
            sc.EnDisableCollideableObject(self.id, False))
        if self.controller_pose_delta is not None:
            self.controller_pose_delta = None
            self.ReleaseCallbacks()

    def EnableGrabbing(self):
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
			self.grabbable_object_handler,
            sc.EnDisableCollideableObject(self.id, True))
