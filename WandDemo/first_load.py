import scene_system as sc
import player, mutable_graphical_object, draggable_object, path_draggable_object, path, draggable_graphics, mech_system, shell, wheel, nixie, shader_helper, lightbulb
import collections
import math

print("IMPORTING FIRST LOAD")

class DummyActor(sc.DelegatingActor[sc.Actor]):
    delegater = sc.Delegater(sc.DelegatingActor[sc.Actor])

    def __init__(self):
        super(DummyActor, self).__init__()
        self.EmbedSelf(self)

    @delegater.RegisterCommand(sc.InputCommand.TICK)
    def HandleTick(self, command_args):
        print("RECEIVED COMMAND OF TYPE TICK", command_args.duration)


def pre_load():
    return {"load_vr": False}

class HackActor(sc.DelegatingActor[sc.NewGraphicsObject_Poseable_ActorImpl]):
    delegater = sc.Delegater(sc.DelegatingActor[sc.NewGraphicsObject_Poseable_ActorImpl])

    def __init__(self):
        super().__init__()
        self.collision = sc.CollisionObject(sc.CollisionObjectType.NORMAL, sc.Shape.MakeSphere(1), sc.Pose())
        self.world = None
        self.simulation_id = None
        self.other_ball_id = None

    def SetPhysicsSim(self, simulation_id):
        self.simulation_id = simulation_id

    def SetOtherBall(self, other_ball_id):
        self.other_ball_id = other_ball_id

    @delegater.RegisterCommand(sc.InputCommand.TICK)
    def HandleTick(self, command_args):
        if self.world is None:
            self.world = self.scene.AskQuery(sc.Target(self.simulation_id), sc.RawQueryArgs(sc.PhysicsSimulationCommand.GET_WORLD)).GetData()
        #b = self.world.CheckCollision(self.collision)
        print("COLLISION FOUND:", self.scene.AskQuery(sc.Target(self.other_ball_id), sc.CheckCollisionQuery(self.world, self.collision)).collision)

    @delegater.RegisterCommand(sc.PoseableCommand.ACCEPT_NEW_POSE)
    def HandleAcceptNewPose(self, command_args):
        print("Hello from in new pose acceptance land!")
        self.collision.SetShape(sc.Shape.MakeSphere(command_args.new_pose.scale[0]))
        self.collision.SetPose(command_args.new_pose)



def first_load(resources):
    print("STARTING FIRST LOAD")
    sc.ParseResources(resources)

    scene = resources["scene"]

    point_obj = sc.CollisionObject(sc.CollisionObjectType.NORMAL, sc.Shape.MakeSphere(0.0001), sc.Pose())

    graphics_resources = scene.AskQuery(
        sc.Target(
            scene.FindByName("GraphicsResources")),
        sc.QueryArgs(
            sc.GraphicsResourceQuery.GRAPHICS_RESOURCE_REQUEST)).GetGraphicsResources()
    graphics_resources.GetEntityHandler().MutableLightSystem("cockpit_lights").MutableAmbientLight().color = sc.Color(1, 1, 1, 0.2)

    sim = sc.PhysicsSimulation()
    ph_scene = sim.GetPhysicsScene()
    phys = sim.GetPhysics()

    box = phys.CreateRigidDynamic(sc.Pose(sc.Location(0, 3, 0), sc.Quaternion.RotationAboutAxis(sc.AxisID.x, 3.14/3.0)))
    box.CreateExclusiveShape(sc.PxBoxGeometry(1, 1, 1), sim.GetDefaultMaterial())
    box.UpdateMassAndInertia(10.0)

    floor_plane = phys.CreatePlane(sc.PxPlane(0, 1, 0, 0), sim.GetDefaultMaterial())

    ph_scene.AddActor(box)
    ph_scene.AddActor(floor_plane)

    while True:
        ph_scene.Simulate(1.0 / 60.0)
        ph_scene.FetchResults()

        actives = ph_scene.GetActiveActors()
        for active in actives:
            cast_active = active.IsPxRigidActor()
            if cast_active:
                print("Active:", cast_active.GetGlobalPose())

    import code
    a = globals()
    a.update(locals())
    code.interact(local = a)


    light = lightbulb.LightBulb(light_system_name = "cockpit_lights", light_number = 0, color = sc.Color(0.5, 0.5, 0.5, 4.25))
    scene.AddActor(light)

    mech = mech_system.MechSystem(
        cannon_details = (
            {
                "starting_pose": sc.Pose(sc.Location(0, 1, 0)),
                "size": 0.5,
            },
        ),
        shell_details = (
#            {
#                "shell_attributes" : shell.ShellAttributes(power = 0.5, color = (1, 1, 0)),
#                "starting_pose" : sc.Pose(sc.Location(1, 1, -0.5)),
#             },
#            {
#                "shell_attributes" : shell.ShellAttributes(power = 1, color = (0, 1, 1), is_flare = True),
#                "starting_pose" : sc.Pose(sc.Location(1, 1, 0)),
#             },
            {
                "shell_attributes" : shell.ShellAttributes(power = 1.5, color = (1, 0, 1)),
                "starting_pose" : sc.Pose(sc.Location(1, 1, 0.5), sc.Quaternion.RotationAboutAxis(sc.AxisID.x, 3.14/3.0)),
                #"starting_pose" : sc.Pose(sc.Location(1, 1, 0.5)),
             },
            ),
        pitch_wheel_args = {
            "pose": sc.Pose(sc.Location(0, 1.25, 1), sc.Quaternion.RotationAboutLocation(sc.Location(1, 0, 0), 3.14/2), sc.Scale(0.35)),
            "crank_height": 0.5,
            },
        yaw_wheel_args = {
            "pose": sc.Pose(sc.Location(1, 1.25, 1), sc.Quaternion.RotationAboutLocation(sc.Location(1, 0, 0), 3.14/2), sc.Scale(0.35)),
            "crank_height": 0.5,
            })
    scene.AddActor(mech)

    p = player.Player(keyboard_and_mouse_controls = not resources["is_vr"])
    player_id = scene.AddActor(p)
    return player_id

