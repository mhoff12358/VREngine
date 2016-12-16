import scene_system as sc
import draggable_object
import math, functools, typing

class Combinable(draggable_object.DraggableObject):
    delegater = sc.Delegater(draggable_object.DraggableObject)

    def __init__(
        self,
        collision_collection_id: sc.ActorId,
        graphics_info: typing.Iterable[typing.Tuple[sc.ActorId, sc.Pose]],
        starting_pose: sc.Pose = sc.Pose(),
        grab_points: typing.Iterable[typing.Tuple[sc.CollisionShape, sc.Pose]] = None,
        combine_shapes: typing.Iterable[typing.Tuple[sc.CollisionShape, sc.Pose]] = None):
        self.collision_collection_id = collision_collection_id
        self.graphics_info = graphics_info
        super().__init__(
            collision_shapes = grab_points if grab_points is not None else [],
            starting_pose = startring_pose,
            draw_ball = True,
            pose_updated_callback = functools.partial(self.Dragged))
        if combine_shapes is None:
            self.combine_shapes = []
        else:
            self.combine_shapes = combine_shapes
        self.combined_objects = [] # list of Combineable, sc.Pose tuples

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        super().HandleAddedToScene(args)
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            sc.Target(self.collision_collection_id),
            sc.AddCollideableObject(
                self.id,
                sc.VectorCollisionShape(zip(*self.combine_shapes)[0])))

    def Dragged(self, global_pose, relative_pose, **kwargs):
        for graphics_actor_id, graphics_offset in self.graphics_info:
            self.scene.MakeCommandAfter(
                self.scene.BackOfNewCommands(),
                sc.Target(graphics_actor_id),
                sc.PlaceComponent(
                    self.graphics_component,
                    graphics_offset.ApplyAfter(relative_pose)))


    def Attach(self, other: Combinable, own_collision_index: int, other_collision_index: int):
        self.graphics_actor_ids.extend(other.graphics_actor_ids)
        other.DisableGrabbing()

        if own_collision_index is not None:
            self.combine_shapes[own_collision_index] = self.combine_shapes[-1]
        if other_collision_index is not None:
            other.combine_shapes[own_collision_index] = other.combine_shapes[-1]
        self.combine_shapes.extend(other.combine_shapes)

        # Remove both old sets of collisions and add the new combined set of collisions.
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            sc.Target(self.collision_collection_id),
            sc.RemoveCollideableObject(
                self.id))
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            sc.Target(self.collision_collection_id),
            sc.RemoveCollideableObject(
                other.id))
        self.scene.MakeCommandAfter(
            self.scene.BackOfNewCommands(),
            sc.Target(self.collision_collection_id),
            sc.AddCollideableObject(
                self.id,
                sc.VectorCollisionShape(zip(*self.combine_shapes)[0])))

        # Trigger all repose based code by moving this object to its existing position.
        self.MoveToPose(self.current_pose)

        
