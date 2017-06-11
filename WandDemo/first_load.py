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
        #if self.world is None:
        #    self.world = self.scene.AskQuery(sc.Target(self.simulation_id), sc.RawQueryArgs(sc.PhysicsSimulationCommand.GET_WORLD)).GetData()
        #b = self.world.CheckCollision(self.collision)
        print("COLLISION FOUND:", self.scene.AskQuery(sc.Target(self.other_ball_id), sc.CheckCollisionQuery(self.collision)).collision)

    @delegater.RegisterCommand(sc.PoseableCommand.ACCEPT_NEW_POSE)
    def HandleAcceptNewPose(self, command_args):
        print("Hello from in new pose acceptance land!")
        self.collision.SetShape(sc.Shape.MakeSphere(command_args.new_pose.scale[0]))
        self.collision.SetPose(command_args.new_pose)

def first_load(resources):
    print("STARTING FIRST LOAD")
    sc.ParseResources(resources)

    scene = resources["scene"]

    import code
    a = globals()
    a.update(locals())
    #code.interact(local = a)

    graphics_resources = scene.AskQuery(
        sc.Target(
            scene.FindByName("GraphicsResources")),
        sc.QueryArgs(
            sc.GraphicsResourceQuery.GRAPHICS_RESOURCE_REQUEST)).GetGraphicsResources()
    graphics_resources.GetEntityHandler().MutableLightSystem("cockpit_lights").MutableAmbientLight().color = sc.Color(1, 1, 1, 0.2)

    physics_object = sc.NewGraphicsObject_PhysicsObject_Poseable_ActorImpl()
    physics_object.RegisterNamedPose("Sphere", sc.PoseData(sc.Pose(), sc.Pose(sc.Scale(0.5)), sc.FreezeBits.SCALE))
    scene.AddActor(physics_object)
    scene.MakeCommandAfter(
        scene.BackOfNewCommands(),
        sc.Target(physics_object.id),
        sc.AddRigidBody(
            "Sphere", sc.RigidBody(sc.Shape.MakeSphere(0.5), sc.Pose(sc.Location(1, 100, 3)), sc.InteractionType(10.0))))
    shader_details = shader_helper.ShaderHelper.Default(pixel_shader_name = "ps_solidcolor", lighting = True)
    scene.MakeCommandAfter(
        scene.BackOfNewCommands(),
        sc.Target(physics_object.id),
        sc.CreateGraphicsObject(
            "basic",
            sc.VectorEntitySpecification((
                sc.EntitySpecification("Shell")
                .SetModel(sc.ModelDetails(
                    sc.ModelIdentifier("sphere.obj"),
                    sc.OutputFormat(
                        sc.ModelModifier(
                            sc.ArrayInt3((0, 1, 2)),
                            sc.ArrayFloat3((1, 1, 1)),
                            sc.ArrayBool2((False, True))),
                        sc.VertexType.all,
                        False)))
                .SetShaders(shader_details)
                .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((1, 1, 1)),)))
                .SetComponent("Sphere"),)),
            sc.VectorComponentInfo((sc.ComponentInfo("", "Sphere"),))))

    collision_sphere = HackActor()
    scene.AddActor(collision_sphere)
    scene.MakeCommandAfter(
        scene.BackOfNewCommands(),
        sc.Target(collision_sphere.id),
        sc.CreateGraphicsObject(
            "basic",
            sc.VectorEntitySpecification((
                sc.EntitySpecification("Shell")
                .SetModel(sc.ModelDetails(
                    sc.ModelIdentifier("sphere.obj"),
                    sc.OutputFormat(
                        sc.ModelModifier(
                            sc.ArrayInt3((0, 1, 2)),
                            sc.ArrayFloat3((1, 1, 1)),
                            sc.ArrayBool2((False, True))),
                        sc.VertexType.all,
                        False)))
                .SetShaders(shader_details)
                .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((1, 0.25, 0.25)),)))
                .SetComponent("Sphere"),)),
            sc.VectorComponentInfo((sc.ComponentInfo("", "Sphere"),))))
    scene.MakeCommandAfter(
        scene.BackOfNewCommands(),
        sc.Target(collision_sphere.id),
        sc.PushNewPose(
            "Sphere",
            "",
            sc.Pose(sc.Location(1, 0.75, 3), sc.Scale(0.05))))
    scene.AddActorToGroup(collision_sphere.id, scene.FindByName("TickRegistry"))

    physics_collection = sc.PhysicsObjectCollection_ActorImpl()
    scene.AddActor(physics_collection)
    scene.MakeCommandAfter(
        scene.BackOfNewCommands(),
        sc.Target(physics_collection.id),
        sc.AddRigidBody(
            "Floor", sc.RigidBody(sc.Shape.MakePlane(sc.Location(0, 1, 0)), sc.Pose(sc.Location(0, 0, 0)))))

    physics_simulation = sc.PhysicsSimulation_ActorImpl()
    physics_sim_id = scene.AddActor(physics_simulation)
    scene.MakeCommandAfter(
        scene.BackOfNewCommands(),
        sc.Target(physics_simulation.id),
        sc.UpdatePhysicsObject(sc.UpdateType.ADD, physics_object.id))
    scene.MakeCommandAfter(
        scene.BackOfNewCommands(),
        sc.Target(physics_simulation.id),
        sc.UpdatePhysicsObject(sc.UpdateType.ADD, physics_collection.id))
    scene.AddActorToGroup(physics_simulation.id, scene.FindByName("TickRegistry"))

    collision_sphere.SetPhysicsSim(physics_sim_id)
    collision_sphere.SetOtherBall(physics_object.id)

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
                "starting_pose" : sc.Pose(sc.Location(1, 1, 0.5), sc.Quaternion.RotationAboutAxis(sc.AxisID.x, 3.14/4)),
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
    scene.AddActor(mech)

    p = player.Player(keyboard_and_mouse_controls = not resources["is_vr"])
    player_id = scene.AddActor(p)
    return player_id

