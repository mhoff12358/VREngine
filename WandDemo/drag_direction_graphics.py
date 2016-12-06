import scene_system as sc
import draggable_object, draggable_graphics
import copy

class DragDirectionGraphics(draggable_graphics.DraggableGraphics):
    def __init__(self,
                 scene: sc.Scene,
                 draggable_actor: draggable_object.DraggableObject,
                 graphics_id: sc.ActorId,
                 graphics_component: str,
                 graphics_pose: sc.Pose = sc.Pose()):
        super().__init__(scene, draggable_actor, graphics_id, graphics_component, graphics_pose)
        self.starting_orientation = copy.copy(draggable_actor.current_pose.location)

    def Dragged(self, global_pose, relative_pose):
        self.scene.MakeCommandAfter(
            self.scene.FrontOfCommands(),
            sc.Target(self.graphics_id),
            sc.PlaceComponent(
                self.graphics_component,
                    sc.Pose(sc.Quaternion.RotationBetweenLocations(self.starting_orientation, relative_pose.location, 1)).ApplyAfter(self.graphics_pose)))
