
class Vertex(object):
    def __init__(self, name):
        self.name = name

class Edge(object):
    def __init__(self, u_name, v_name):
        self.u_name = u_name
        self.v_name = v_name

def load_graph():
    return {
        "vertices" : [
            Vertex("v1"),
            Vertex("v2"),
            Vertex("v3"),
        ],
        "edges": [
            Edge("v1", "v2"),
            Edge("v1", "v3"),
        ]}