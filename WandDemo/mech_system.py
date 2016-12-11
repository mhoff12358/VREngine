import scene_system as sc
import shell, cannon, wheel
import math, functools

class MechSystem(sc.DelegatingActor):
    delegater = sc.Delegater(sc.DelegatingActor)

    default_cannon_details = {
        "starting_pose": sc.Pose(),
        "size": 1
        }
    default_shell_details = {
        "starting_pose": sc.Pose(),
        "size": 0.1
        }

    def __init__(self, cannon_details = (), shell_details = (), pitch_wheel_args = {}, yaw_wheel_args = {}):
        super().__init__()
        self.shell_reposed_callback = functools.partial(self.ShellReposed)

        self.cannons = list(map(self.CannonDetailsToCannon, cannon_details))
        self.shells = list(map(self.ShellDetailsToShell, shell_details))
        pitch_wheel_args = dict(pitch_wheel_args)
        pitch_wheel_args["spun_callbacks"] = [functools.partial(cannon.UpdatePitch) for cannon in self.cannons]
        self.pitch_wheel = wheel.Wheel(**pitch_wheel_args)
        yaw_wheel_args = dict(yaw_wheel_args)
        yaw_wheel_args["spun_callbacks"] = [functools.partial(cannon.UpdateYaw) for cannon in self.cannons]
        self.yaw_wheel = wheel.Wheel(**yaw_wheel_args)

    def ShellReposed(self, shell):
        shell_loading_collision = shell.GetLoadingCollision()
        for cannon_actor in self.cannons:
            if shell_loading_collision.Intersect(cannon_actor.GetLoadingCollision()):
                if cannon_actor.TryLoadShell(shell.id):
                    return

    def CannonDetailsToCannon(self, cannon_details):
        actual_cannon_details = {}
        actual_cannon_details.update(self.default_cannon_details)
        actual_cannon_details.update(cannon_details)
        return cannon.Cannon(**actual_cannon_details)

    def ShellDetailsToShell(self, shell_details):
        actual_shell_details = {}
        actual_shell_details.update(self.default_shell_details)
        actual_shell_details.update(shell_details)
        return shell.Shell(reposed_callback = self.shell_reposed_callback, **actual_shell_details)

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        for cannon in self.cannons:
            self.scene.AddActor(cannon)
        for shell in self.shells:
            self.scene.AddActor(shell)
        self.scene.AddActor(self.pitch_wheel)
        self.scene.AddActor(self.yaw_wheel)
