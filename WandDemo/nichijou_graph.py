
import random

class Vertex(object):
    def __init__(self, name):
        self.name = name

class Edge(object):
    def __init__(self, u_name, v_name):
        self.u_name = u_name
        self.v_name = v_name

def load_graph():
    num_vertices = 50
    edge_list = []
    for i in range(num_vertices):
        edge_list.append(Edge("v"+str(random.randrange(num_vertices)), "v"+str(random.randrange(num_vertices))))
    return {
        "configuration" : {
            "graph_radius" : 1,
            "vertex_configuration" : {
                "radius" : 0.025,
                },
            "edge_configuration" : {
                "width" : 0.0125
                },
            },
        "vertices" : list(Vertex("v" + str(i)) for i in range(num_vertices)),
        "edges" : list(
            edge_list
            #Edge("v" + str(random.randrange(num_vertices)), "v" + str(random.randrange(num_vertices))) for i in range(num_vertices)
        )}