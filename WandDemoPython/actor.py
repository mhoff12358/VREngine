from scene_system_ import *

pyactor_identifier = "PyActor_"
key = None
keys_to_mutate = []
for key in globals():
    if key.startswith(pyactor_identifier):
        keys_to_mutate.append(key)

for key in keys_to_mutate:
    subclass = globals().pop(key)
    key = key[len(pyactor_identifier):]
    class Actor(subclass):
        def __init__(self):
            super().__init__()
            self.EmbedSelf(self)
    globals()[key] = Actor

globals().pop("key")
globals().pop("keys_to_mutate")
globals().pop("pyactor_identifier")

Actor = ActorImpl
