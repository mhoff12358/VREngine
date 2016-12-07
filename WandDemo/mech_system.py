import scene_system as sc
import shell, cannon
import math, functools

class MechSystem(sc.DelegatingActor):
    command_delegation = sc.DelegatingActor.GetDefaultDelegation()
    delegater = sc.delegate_for_command(command_delegation)

    default_cannon_details = {
        "starting_pose": sc.Pose(),
        "size": 1
        }
    default_shell_details = {
        "starting_pose": sc.Pose(),
        "size": 1
        }

    def __init__(self, cannon_details = (), shell_details = ()):
        super().__init__()
        self.cannon_details = tuple(cannon_details)
        self.shell_details = tuple(shell_details)
        self.cannons = []
        self.shells = []
        self.shell_reposed_callback = functools.partial(self.ShellReposed)

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

    @delegater(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        self.cannons = list(map(self.CannonDetailsToCannon, self.cannon_details))
        self.cannon_details = None
        for cannon in self.cannons:
            self.scene.AddActor(cannon)
        self.shells = list(map(self.ShellDetailsToShell, self.shell_details))
        self.shell_details = None
        for shell in self.shells:
            self.scene.AddActor(shell)
