import scene_system as sc
import draggable_graphics, draggable_object, path, drag_direction_graphics, shader_helper
import math, functools, typing, copy

print("IMPORTING SHELL")

class ShellAttributes(object):
    def __init__(self, power, color = (0.75, 0.75, 0.75), is_flare = False, spent = False):
        self.power = power
        self.color = color
        self.is_flare = is_flare
        self.spent = spent

class ShellQueries(sc.QueryRegistration):
    GET_ATTRIBUTES = ()

class ShellCommands(sc.CommandRegistration):
    LOAD = ()
    FIRE = ()

    UPDATE_LOAD_POSE = ()

class RespondShellAttributes(sc.QueryResult):
    def __init__(self, shell_attributes: ShellAttributes):
        super().__init__(ShellQueries.GET_ATTRIBUTES)
        self.shell_attributes = shell_attributes

class LoadShell(sc.CommandArgs):
    def __init__(self, load_pose: sc.Pose):
        super().__init__(ShellCommands.LOAD)
        self.load_pose = copy.copy(load_pose)

class UpdateLoadPose(sc.CommandArgs):
    def __init__(self, load_pose: sc.Pose):
        super().__init__(ShellCommands.UPDATE_LOAD_POSE)
        self.load_pose = copy.copy(load_pose)

class FireShell(sc.CommandArgs):
    def __init__(self):
        super().__init__(ShellCommands.FIRE)

class Shell(sc.DelegatingActor[sc.NewGraphicsObject_PrintNewPoses_PhysicsObject_Poseable]):
    delegater = sc.Delegater(sc.DelegatingActor[sc.NewGraphicsObject_PrintNewPoses_PhysicsObject_Poseable])

    spent_color = (0.1, 0.1, 0.1)

    def __init__(self, shell_attributes: ShellAttributes, starting_pose: sc.Pose = sc.Pose(), size: float = 1, reposed_callback = None):
        super().__init__()
        self.shell_attributes = shell_attributes
        self.draggable_shell = None # type : draggable_object.DraggableObject
        self.shell_wrapper = None # type : draggable_graphics.DraggableGraphics
        self.shell_pose = sc.Pose()
        self.loading_collision = sc.CollisionShape(sc.Pose(), 0.1)
        self.reposed_callback = reposed_callback

        self.starting_pose = copy.copy(starting_pose)
        self.size = size

    def SetShellPose(self, shell_pose):
        self.shell_pose = shell_pose
        self.loading_collision.SetPose(self.shell_pose)
        if self.reposed_callback is not None:
            self.reposed_callback(self)

    def ShellDragged(self, global_pose, relative_pose):
        self.SetShellPose(relative_pose)

    def GetLoadingCollision(self):
        return self.loading_collision

    def SetSize(self, size):
        self.size = size
        self.FreezePose("Cylinder", sc.Pose(sc.Scale(self.size)), sc.FreezeBits.SCALE)

    @delegater.RegisterCommand(sc.CommandType.ADDED_TO_SCENE)
    def HandleAddedToScene(self, args):
        self.LoadGraphicsResources()
        self.CreatePhysicsResources()
        self.RegisterNamedPose("Cylinder", sc.PoseData())
        self.SetSize(self.size)
        self.PushNewPose("Cylinder", self.starting_pose)

    @delegater.RegisterCommand(ShellCommands.LOAD)
    def HandleLoadShell(self, args: LoadShell):
        pass
        #self.draggable_shell.MoveToPose(args.load_pose)
        #self.draggable_shell.DisableGrabbing()

    @delegater.RegisterCommand(ShellCommands.UPDATE_LOAD_POSE)
    def HandleUpdateLoadPose(self, args: UpdateLoadPose):
        pass
        #self.draggable_shell.MoveToPose(args.load_pose)

    @delegater.RegisterCommand(ShellCommands.FIRE)
    def HandleFireShell(self, args):
        self.draggable_shell.EnableGrabbing()
        self.shell_attributes.spent = True
        self.HandleCommand(
            sc.SetEntityShaderValues(
                "Shell",
                sc.ShaderSettingsValue((sc.VectorFloat(self.spent_color),))))

    @delegater.RegisterQuery(ShellQueries.GET_ATTRIBUTES)
    def HandleGetAttributes(self, args):
        return RespondShellAttributes(copy.copy(self.shell_attributes))

    def LoadGraphicsResources(self):
        shader_details = shader_helper.ShaderHelper.Default(pixel_shader_name = "ps_solidcolor", lighting = True)
        self.HandleCommand(
            sc.CreateGraphicsObject(
                "basic",
                sc.VectorEntitySpecification((
                    sc.EntitySpecification("Shell")
                    .SetModel(sc.ModelDetails(
                        sc.ModelIdentifier("cylinder.obj"),
                        sc.OutputFormat(
                            sc.ModelModifier(
                                sc.ArrayInt3((0, 1, 2)),
                                sc.ArrayFloat3((1, 1, 1)),
                                sc.ArrayBool2((False, True))),
                            sc.VertexType.all,
                            False)))
                    .SetShaders(shader_details)
                    .SetShaderSettingsValue(sc.ShaderSettingsValue((sc.VectorFloat(self.shell_attributes.color),)))
                    .SetComponent("Cylinder"),)),
                sc.VectorComponentInfo((sc.ComponentInfo("", "Cylinder"),))))

    def CreatePhysicsResources(self):
        sim = sc.GlobalPhysicsSimulation()
        box = sim.GetPhysics().CreateRigidDynamic(self.starting_pose)
        box.CreateExclusiveShape(sc.PxBoxGeometry(1, 1, 1), sim.GetDefaultMaterial())
        box.UpdateMassAndInertia(10.0)
        self.HandleCommand(sc.AddRigidBody(
            "Cylinder", box))
