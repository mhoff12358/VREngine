
import random

class TimeRange(object):
    def __init__(self, min_value, max_value, min_inclusive = True, max_inclusive = False):
        self.min_value = min_value
        self.max_value = max_value
        self.min_inclusive = min_inclusive
        self.max_inclusive = max_inclusive

class Vertex(object):
    def __init__(self, name, time_range, texture_name):
        self.name = name
        self.texture_name = texture_name
        self.time_range = time_range

class Edge(object):
    def __init__(self, time_range, u_name, v_name):
        self.u_name = u_name
        self.v_name = v_name
        self.time_range = time_range

def load_graph():
    character_names = [
        "hakase",
        "mai",
        "mio",
        "misato",
        "nano",
        "yuuko",
        ]
    return {
        "configuration" : {
            # Visual configuration values
            "graph_radius" : 1,
            "vertex_configuration" : {
                "radius" : 0.25,
                "shader_name" : "sprite_textured_circle.hlsl",
                },
            "edge_configuration" : {
                "width" : 0.025,
                "vertex_spacing" : 0.075,
                },
            # Interaction configuration values
            "timeline_width" : 5,
            },
        "vertices" : list(Vertex(name = name, time_range = TimeRange(0, 4), texture_name = "nichijou_images/"+name+".png") for name in character_names),
        "edges" : [ Edge(TimeRange(0, 5), "nano", "hakase") ],
        }