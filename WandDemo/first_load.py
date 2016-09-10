import scene_system as sc
import collections

class DelegatingActor(sc.Actor):
    def DefaultHandleCommand(self, command_args):
        print("Failed to provide a handler for command args of type", command_args.Type())

    @classmethod
    def GetDefaultHandleCommand(cls):
        return cls.DefaultHandleCommand

    @classmethod
    def GetDefaultDelegation(cls):
        return collections.defaultdict(DelegatingActor.GetDefaultHandleCommand)

    def HandleCommand(self, command_args):
        self.command_delegation[command_args.Type()](self, command_args)

        
DelegatingActor.command_delegation = DelegatingActor.GetDefaultDelegation()

class DummyActor(DelegatingActor):
    command_delegation = DelegatingActor.GetDefaultDelegation()

    def __init__(self):
        super(DummyActor, self).__init__()
        self.EmbedSelf(self)

    def AnswerQuery(self, query_args):
        print("RECEIVED QUERY ARGS OF TYPE", query_args.Type())
        return QueryResult(query_args.Type())

    def HandleTICK(self, command_args):
        print("BLARG", command_args.blarg(), command_args.blargwrap())
        print("RECEIVED COMMAND OF TYPE TICK", command_args.duration)
    command_delegation[sc.InputCommand.TICK] = HandleTICK

    #def HandleCommand(self, command_args):
    #    print(sc.InputCommand.TICK)
    #    print("RECEIVED COMMAND OF TYPE", sc.SceneSystemGlobals.command_registry.LookupName(command_args.Type()), command_args.Type())

def first_load(resources):
    print("STARTING FIRST LOAD")
    
    a1 = sc.f1()
    a2 = sc.f2()
    a3 = sc.f3()

    import code
    code.interact(local = locals())

    sc.ParseResources(resources)

    sc.Actor.SetScene(resources["scene"])

    scene = sc.Actor.GetScene()
    dummy_actor_id = scene.AddActor(DummyActor())
    tick_group = scene.FindByName("TickRegistry")
    scene.AddActorToGroup(dummy_actor_id, tick_group)

    return dummy_actor_id

def dump_thing(thing):
    print(thing)