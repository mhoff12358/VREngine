from wand_demo_py import *

class DummyActor(Actor):
    def __init__(self):
        super(DummyActor, self).__init__()
        self.EmbedSelf(self)

    def AnswerQuery(self, query_args):
        print("RECEIVED QUERY ARGS OF TYPE", query_args.Type())
        return QueryResult(query_args.Type())

def first_load(scene):
    print("STARTING FIRST LOAD")
    
    dummy_actor_id = scene.AddActor(DummyActor())

    return dummy_actor_id