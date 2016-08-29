
import random
from wand_demo_py import *
#import code
#code.interact(local = locals())

class Timeline(object):
    def __init__(self, scenes, scene_length):
        self.scenes = scenes
        self.scene_length = scene_length

class Scene(object):
    def __init__(self, image_name, character_names):
        self.image_name = image_name
        self.time_range = TimeRange(0, 0)
        self.character_names = character_names

    def SetTimeRange(self, time_range):
        self.time_range = time_range

class TimeRangeUnion(object):
    def __init__(self, time_ranges):
        self.time_ranges = time_ranges

    def AddRange(self, time_range):
        self.time_ranges.append(time_range)

    def InRange(self, time_value):
        for time_range in self.time_ranges:
            if time_range.InRange(time_value):
                return True
        return False

class TimeRange(object):
    def __init__(self, min_value, max_value, min_inclusive = True, max_inclusive = False):
        self.min_value = min_value
        self.max_value = max_value
        self.min_inclusive = min_inclusive
        self.max_inclusive = max_inclusive

    def InRange(self, time_value):
        after_min = (time_value > self.min_value) or ((time_value == self.min_value) and self.min_inclusive)
        before_max = (time_value < self.max_value) or ((time_value == self.max_value) and self.max_inclusive)
        return after_min and before_max

class Graph(object):
    def __init__(self, vertices, edges):
        self.vertices = vertices


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
            "timeline_colors" : (
                (1.0, 1.0, 1.0, 1.0),
                (0.0, 0.0, 0.0, 1.0),
                ),
            # Interaction configuration values
            "timeline_length" : 5,
            },
        "vertices" : list(Vertex(name = name, time_range = TimeRange(0, 4), texture_name = "nichijou_images/"+name+".png") for name in character_names),
        "edges" : [ Edge(TimeRange(0, 2.5), "nano", "hakase") ],
        }