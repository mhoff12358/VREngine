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
                 delta_pose: sc.Pose = sc.Pose()):
        self.scene = scene
        self.draggable_actor = draggable_actor
        self.graphics_id = graphics_id
        self.graphics_component = graphics_component
        self.graphics_pose = copy.copy(graphics_pose)
        self.current_dragged_pose = sc.Pose()
        self.delta_pose = copy.copy(delta_pose)

        def DraggedClosure(*args, **kwargs):
            self.Dragged(*args, **kwargs)
        self.draggable_actor.SetPoseUpdatedCallback(DraggedClosure)

    def Dragged(self, global_pose, relative_pose):
        self.current_dragged_pose = copy.copy(relative_pose)
        self.ReposeGraphics()

    def SetGraphicsPose(self, new_pose):
        self.graphics_pose = copy.copy(new_pose)
        self.ReposeGraphics()

    def ReposeGraphics(self):
        self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.graphics_id),
            sc.PlaceComponent(
                self.graphics_component,
                self.graphics_pose.ApplyAfter(self.current_dragged_pose.UnapplyAfter(self.delta_pose))))
