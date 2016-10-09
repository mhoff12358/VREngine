import scene_system as sc
import collections
import scripts.player

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

    sc.Actor.SetScene(resources["scene"])

    scene = sc.Actor.GetScene()
    #dummy_actor_id = scene.AddActor(DummyActor())
    #tick_group = scene.FindByName("TickRegistry")
    #scene.AddActorToGroup(dummy_actor_id, tick_group)

    p = scripts.player.Player()
    player_id = scene.AddActor(p)
    return player_id

def dump_thing(thing):
    print(thing)