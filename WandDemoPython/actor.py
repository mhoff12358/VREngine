from scene_system_ import *

key = ""
v = ""
for key, v in globals().items():
    if key.startswith("PyActor-"):
        class Actor(v):
            def __init__(self):
                super().__init__()
                self.EmbedSelf(self)
        v = Actor

globals().pop("key")
globals().pop("v")

Actor = PyActor_ActorImpl
