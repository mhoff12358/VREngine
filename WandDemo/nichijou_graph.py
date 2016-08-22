
import random

class Vertex(object):
    def __init__(self, name, texture_name):
        self.name = name
        self.texture_name = texture_name

class Edge(object):
    def __init__(self, u_name, v_name):
        self.u_name = u_name
        self.v_name = v_name

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
            "graph_radius" : 1,
            "vertex_configuration" : {
                "radius" : 0.25,
                "shader_name" : "sprite_textured_circle.hlsl",
                },
            "edge_configuration" : {
                "width" : 0.025,
                "vertex_spacing" : 0.075,
                },
            },
        "vertices" : list(Vertex(name, "nichijou_images/"+name+".png") for name in character_names),
        "edges" : [ Edge("nano", "hakase") ],
        }