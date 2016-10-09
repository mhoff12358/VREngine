import scene_system as sc

class Player(sc.DelegatingActor):
    command_delegation = sc.DelegatingActor.GetDefaultDelegation()
    delegater = sc.delegate_for_command(command_delegation)

    def __init__(self):
        super(sc.DelegatingActor, self).__init__()
        self.EmbedSelf(self)
        
    @delegater(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddToScene(self, args):
        print("Player added to the scene")
        scene = self.GetScene()
        resource_pool = scene.FindByName("GraphicsResources").resource_pool
        import pdb; pdb.set_trace()
        #scene.AddActorToGroup(self.id, scene.FindByName("ControlsRegistry"))

    @delegater(sc.IOInterfaceCommand.LISTEN_MOUSE_MOTION)
    def HandleMouseMovement(self, args):
        pass
        #print("MOUSE MOTION", args.motion, sc.x(args.motion), sc.y(args.motion))