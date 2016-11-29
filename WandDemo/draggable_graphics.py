import scene_system as sc
import draggable_object
import copy

class DraggableGraphics(object):
    def __init__(self,
                 scene: sc.Scene,
                 draggable_actor: draggable_object.DraggableObject,
                 graphics_id: sc.ActorId,
                 graphics_component: str,
                 graphics_pose: sc.Pose = sc.Pose(),
                 parent_pose: sc.Pose = sc.Pose()):
        self.scene = scene
        self.draggable_actor = draggable_actor
        self.graphics_id = graphics_id
        self.graphics_component = graphics_component
        self.graphics_pose = copy.copy(graphics_pose)
        self.parent_pose = copy.copy(parent_pose)

        def DraggedClosure(*args, **kwargs):
            self.Dragged(*args, **kwargs)
        self.draggable_actor.SetPoseUpdatedCallback(DraggedClosure)

    def Dragged(self, global_pose, relative_pose):
        self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.graphics_id),
            sc.PlaceComponent(
                self.graphics_component,
                self.graphics_pose.ApplyAfter(relative_pose)))