import scene_system as sc

class PointLight(object):
    def __init__(self, location: sc.Location = sc.Location(0, 0, 0), color: sc.Color = sc.Color(1, 1, 1, 1), light_system_name = "", number = 0, entity_handler: sc.EntityHandler = None, scene: sc.Scene = None):
        self.entity_handler = entity_handler
        if self.entity_handler is None:
            graphics_resources = scene.AskQuery(
                sc.Target(
                    scene.FindByName("GraphicsResources")), sc.QueryArgs(
                    sc.GraphicsResourceQuery.GRAPHICS_RESOURCE_REQUEST)).GetGraphicsResources()
            self.entity_handler = graphics_resources.GetEntityHandler()
        self.light_system_name = light_system_name
        self.number = number
        self.location = location
        self.color = color
        self.Push()

    def GetLight(self):
        light_system = self.entity_handler.MutableLightSystem(self.light_system_name)
        point_lights = light_system.MutablePointLights()
        point_light = point_lights[self.number]
        return point_light

    def PushColor(self):
        self.GetLight().color = self.color

    def PushLocation(self):
        self.GetLight().location = self.location

    def Push(self):
        self.GetLight().color = self.color
        self.GetLight().location = self.location

    def SetColor(self, new_color: sc.Color):
        self.color = new_color
        self.PushColor()

    def SetLocation(self, new_location: sc.Location):
        self.location = new_location
        self.PushLocation()