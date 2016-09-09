import scene_system as sc

class DummyActor(sc.Actor):
    def __init__(self):
        super(DummyActor, self).__init__()
        self.EmbedSelf(self)

    def AnswerQuery(self, query_args):
        print("RECEIVED QUERY ARGS OF TYPE", query_args.Type())
        return QueryResult(query_args.Type())

    def HandleCommand(self, command_args):
        print("RECEIVED COMMAND OF TYPE", sc.SceneSystemGlobals.command_registry.LookupName(command_args.Type()), command_args.Type())

def first_load(resources):
    print("STARTING FIRST LOAD")
    sc.ParseResources(resources)

    sc.Actor.SetScene(resources["scene"])

    scene = sc.Actor.GetScene()
    dummy_actor_id = scene.AddActor(DummyActor())
    tick_group = scene.FindByName("TickRegistry")
    scene.AddActorToGroup(dummy_actor_id, tick_group)

    return dummy_actor_id