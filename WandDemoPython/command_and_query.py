from scene_system_ import *

class CommandArgs(object):
    def __init__(self, type):
        self.type = type

    def Type(self):
        return self.type;

class QueryArgs(object):
    def __init__(self, type):
        self.type = type

    def Type(self):
        return self.type;

class QueryResult(object):
    def __init__(self, type):
        self.type = type

    def Type(self):
        return self.type;
