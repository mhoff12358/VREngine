import scene_system as sc
import player, mutable_graphical_object, draggable_object, path_draggable_object, path, draggable_graphics, mech_system, shell
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

    b = scene.AddActor(path_draggable_object.PathDraggableObject(
        path.Path((path.Line(sc.Location(0, 0.25, 0), sc.Location(0, 0.5, 0), 0.1),
                   path.Line(sc.Location(0, 0.5, 0), sc.Location(1, 0.5, 0), 0.1),
                   path.Line(sc.Location(1, 0.5, 0), sc.Location(0, 1, 0), 0.1))),
        path_sample_rate=1, draw_path = True, draw_ball = True))

    # Makes the circular path
    num_samples = 10
    theta_samples = tuple(
        math.pi * 2 * i / num_samples for i in range(
            num_samples + 1))
    lines = tuple(
        path.Line(
            sc.Location(
                math.cos(
                    theta_samples[i]),
                1,
                math.sin(
                    theta_samples[i])),
            sc.Location(
                math.cos(
                    theta_samples[
                        i + 1]),
                1,
                math.sin(
                    theta_samples[
                        i + 1])),
            0.1) for i in range(num_samples))
    drag_circle = path_draggable_object.PathDraggableObject(
        path.Path(lines, circular=True), path_sample_rate=1,
        draw_path = True, draw_ball = True)
    scene.AddActor(drag_circle)


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
                "starting_pose" : sc.Pose(sc.Location(-1, 1, -0.5)),
             },
            {
                "shell_attributes" : shell.ShellAttributes(power = 1, color = (0, 1, 1), is_flare = True),
                "starting_pose" : sc.Pose(sc.Location(-1, 1, 0)),
             },
            {
                "shell_attributes" : shell.ShellAttributes(power = 1.5, color = (1, 0, 1)),
                "starting_pose" : sc.Pose(sc.Location(-1, 1, 0.5)),
             }
            ))
    scene.AddActor(mech)

    import code
    a = globals()
    a.update(locals())
    #code.interact(local = a)

    p = player.Player()
    player_id = scene.AddActor(p)
    return player_id

