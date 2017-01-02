import scene_system as sc
import player, mutable_graphical_object, draggable_object, path_draggable_object, path, draggable_graphics, mech_system, shell, wheel, nixie
import collections
import math


class DummyActor(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    def __init__(self):
        super(DummyActor, self).__init__()
        self.EmbedSelf(self)

    @delegater.RegisterCommand(sc.InputCommand.TICK)
    def HandleTICK(self, command_args):
        print("RECEIVED COMMAND OF TYPE TICK", command_args.duration)


def first_load(resources):
    print("STARTING FIRST LOAD")
    sc.ParseResources(resources)

    scene = resources["scene"]

    nixie9 = nixie.Nixie(9, sc.Pose(sc.Location(0, 0, 0), sc.Quaternion.RotationAboutAxis(sc.AxisID.x, 3.14/2)))
    scene.AddActor(nixie9)

    mech = mech_system.MechSystem(
        cannon_details = (
            {
                "starting_pose": sc.Pose(sc.Location(0, 1, 0)),
                "size": 0.5,
            },
        ),
        shell_details = (
            {
                "shell_attributes" : shell.ShellAttributes(power = 0.5, color = (1, 1, 0)),
                "starting_pose" : sc.Pose(sc.Location(1, 1, -0.5)),
             },
            {
                "shell_attributes" : shell.ShellAttributes(power = 1, color = (0, 1, 1), is_flare = True),
                "starting_pose" : sc.Pose(sc.Location(1, 1, 0)),
             },
            {
                "shell_attributes" : shell.ShellAttributes(power = 1.5, color = (1, 0, 1)),
                "starting_pose" : sc.Pose(sc.Location(1, 1, 0.5)),
             }
            ),
        pitch_wheel_args = {
            "pose": sc.Pose(sc.Location(0, 1.25, 1), sc.Quaternion.RotationAboutLocation(sc.Location(1, 0, 0), 3.14/2), sc.Scale(0.35)),
            "crank_height": 0.5,
            },
        yaw_wheel_args = {
            "pose": sc.Pose(sc.Location(1, 1.25, 1), sc.Quaternion.RotationAboutLocation(sc.Location(1, 0, 0), 3.14/2), sc.Scale(0.35)),
            "crank_height": 0.5,
            })
    #scene.AddActor(mech)

    import code
    a = globals()
    a.update(locals())
    #code.interact(local = a)

    p = player.Player()
    player_id = scene.AddActor(p)
    return player_id

