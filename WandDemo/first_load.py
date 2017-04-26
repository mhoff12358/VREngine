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
    def HandleTICK(self, command_args):
        print("RECEIVED COMMAND OF TYPE TICK", command_args.duration)


def pre_load():
    return {"load_vr": False}

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

#    physics_object = sc.NewGraphicsObject_PhysicsObject_Poseable_ActorImpl()
#    scene.AddActor(physics_object)
#    scene.MakeCommandAfter(
#        scene.BackOfNewCommands(),
#        sc.Target(physics_object.id),
#        sc.AddRigidBody(
#            "", sc.RigidBody(sc.Shape.MakeSphere(1.0), sc.Pose(sc.Location(1, 10, 3)), 10.0)))
##    scene.MakeCommandAfter(
##        scene.BackOfNewCommands(),
##        sc.Target(physics_object.id),
##        sc.CreateGraphicsObject(
##            "basic",
##            sc.VectorEntitySpecification((
##                sc.EntitySpecification("Shell")
##                .SetModel(sc.ModelDetails(
##                    sc.ModelIdentifier("sphere.obj|Sphere")))
##                .SetShaders(sc.ShaderDetails(
##                    sc.VectorShaderIdentifier((
##                        sc.ShaderIdentifier("vs_location_apply_mvp.cso", sc.ShaderStage.Vertex(), sc.VertexType.location),
##                        sc.ShaderIdentifier("ps_solidcolor.cso", sc.ShaderStage.Pixel())))))
##                .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((1, 1, 0, 1)),)))
##                .SetComponent("Sphere"),)),
##            sc.VectorComponentInfo((sc.ComponentInfo("", "Sphere"),))))
#    scene.MakeCommandAfter(
#        scene.BackOfNewCommands(),
#        sc.Target(physics_object.id),
#        sc.PlaceComponent(
#            "Sphere",
#            sc.Pose(sc.Location(1, 10, 3))))
#
#    physics_collection = sc.PhysicsObjectCollection_ActorImpl()
#    scene.AddActor(physics_collection)
#    scene.MakeCommandAfter(
#        scene.BackOfNewCommands(),
#        sc.Target(physics_collection.id),
#        sc.AddRigidBody(
#            "Floor", sc.RigidBody(sc.Shape.MakePlane(sc.Location(0, 1, 0)), sc.Pose(sc.Location(0, 0, 0)))))
#
#    physics_simulation = sc.PhysicsSimulation_ActorImpl()
#    scene.AddActor(physics_simulation)
#    scene.MakeCommandAfter(
#        scene.BackOfNewCommands(),
#        sc.Target(physics_simulation.id),
#        sc.UpdatePhysicsObject(sc.UpdateType.ADD, physics_object.id))
#    scene.MakeCommandAfter(
#        scene.BackOfNewCommands(),
#        sc.Target(physics_simulation.id),
#        sc.UpdatePhysicsObject(sc.UpdateType.ADD, physics_collection.id))
#    scene.AddActorToGroup(physics_simulation.id, scene.FindByName("TickRegistry"))

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

