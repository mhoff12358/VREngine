from scene_system_ import *

def SetGlobalPhysicsSimulation(simulation):
    PyActor_PhysicsSimulation_ActorImpl.global_simulation = simulation

def GlobalPhysicsSimulation():
    return PyActor_PhysicsSimulation_ActorImpl.global_simulation

