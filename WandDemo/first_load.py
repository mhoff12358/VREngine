import scene_system as sc
import player, mutable_graphical_object, draggable_object, path_draggable_object, path, draggable_graphics
import collections
import math


class DummyActor(sc.DelegatingActor):
    command_delegation = sc.DelegatingActor.GetDefaultDelegation()

    def __init__(self):
        super(DummyActor, self).__init__()
        self.EmbedSelf(self)

    def AnswerQuery(self, query_args):
        print("RECEIVED QUERY ARGS OF TYPE", query_args.Type())
        return QueryResult(query_args.Type())

    def HandleTICK(self, command_args):
        print("RECEIVED COMMAND OF TYPE TICK", command_args.duration)
    command_delegation[sc.InputCommand.TICK] = HandleTICK


def first_load(resources):
    print("STARTING FIRST LOAD")
    sc.ParseResources(resources)

    scene = resources["scene"]

    #graphics_object = mutable_graphical_object.MutableGraphicalObject()
    # scene.AddActor(graphics_object)

    scene.AddActor(path_draggable_object.PathDraggableObject(
        path.Path((path.Line(sc.Location(0, 0.25, 0), sc.Location(0, 0.5, 0), 0.1),
                   path.Line(sc.Location(0, 0.5, 0), sc.Location(1, 0.5, 0), 0.1),
                   path.Line(sc.Location(1, 0.5, 0), sc.Location(0, 1, 0), 0.1))),
        path_sample_rate=1))

    num_samples = 10
    theta_samples = tuple(
        math.pi * 2 * i / num_samples for i in range(
            num_samples + 1))
    lines = tuple(
        path.Line(
            sc.Location(
                math.cos(
                    theta_samples[i]),
                2,
                math.sin(
                    theta_samples[i])),
            sc.Location(
                math.cos(
                    theta_samples[
                        i + 1]),
                2,
                math.sin(
                    theta_samples[
                        i + 1])),
            0.1) for i in range(num_samples))
    scene.AddActor(path_draggable_object.PathDraggableObject(
        path.Path(lines, circular=True), path_sample_rate=1))

    drag_obj = draggable_object.DraggableObject(
        ((sc.CollisionShape(sc.Pose(), 0.1), sc.Pose()),),
        sc.Pose(sc.Location(0, 1, 0)))
    scene.AddActor(drag_obj)
    latest_command = scene.FrontOfCommands()
    square_id = scene.AddAndConstructGraphicsObject().id
    square_pose = sc.Pose(sc.Location(0, 1, 0), sc.Scale(0.25))
    latest_command = scene.MakeCommandAfter(
        latest_command,
        sc.Target(square_id),
        sc.CreateGraphicsObject(
            "basic",
                sc.VectorEntitySpecification((
                    sc.EntitySpecification("Square")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier("square.obj|Square")))
                    .SetShaders(sc.ShaderDetails(
                        sc.VectorShaderIdentifier((
                            sc.ShaderIdentifier("textured.hlsl", sc.ShaderStage.Vertex(), sc.VertexType.texture),
                            sc.ShaderIdentifier("textured.hlsl", sc.ShaderStage.Pixel())))))
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat((0, 0, 0)), sc.VectorFloat((1,)))))
                    .SetTextures(sc.VectorIndividualTextureDetails((sc.IndividualTextureDetails("terrain.png", sc.ShaderStages.All(), 0, 0),)))
                    .SetComponent("Square"),)),
                sc.VectorComponentInfo((sc.ComponentInfo("", "Square"),))))
    drag_graph = draggable_graphics.DraggableGraphics(
        scene = scene,
        draggable_actor = drag_obj,
        graphics_id = square_id,
        graphics_component = "Square",
        graphics_pose = sc.Pose(sc.Scale(0.25)))


    import code
    a = globals()
    a.update(locals())
    #code.interact(local = a)

    p = player.Player()
    player_id = scene.AddActor(p)
    return player_id


def dump_thing(thing):
    print(thing)
