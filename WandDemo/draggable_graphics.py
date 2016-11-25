import scene_system as sc
import draggable_object

class DraggableGraphics(object):
    def __init__(self,
                 scene: sc.Scene,
                 draggable_actor: draggable_object.DraggableObject,
                 graphics_id: sc.ActorId,
                 graphics_component: str,
                 graphics_pose: sc.Pose = sc.Pose()):
        self.scene = scene
        self.draggable_actor = draggable_actor
        self.graphics_id = graphics_id
        self.graphics_component = graphics_component
        self.graphics_pose = graphics_pose

        def DraggedClosure(new_pose):
            self.Dragged(new_pose)
        self.draggable_actor.SetPoseUpdatedCallback(DraggedClosure)

    def Dragged(self, new_pose):
        self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.graphics_id),
            sc.PlaceComponent(
                self.graphics_component,
                new_pose.ApplyAfter(self.graphics_pose)))