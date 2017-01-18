import sys, builtins

class ReloadImport(object):
    def __init__(self):
        self.original_modules = sys.modules.copy()

        self.import_fn = builtins.__import__

        self.additional_modules = set()

    def Install(self):
        builtins.__import__ = self.Import
        
    def Uninstall(self):
        builtins.__import__ = self.import_fn

    def Import(self, name, *args, **kwargs):
        result = self.import_fn(name, *args, **kwargs)
        self.additional_modules.add(name)
        return result

    def Reload(self):
        for deleted_module in self.additional_modules:
            if not deleted_module in self.original_modules:
                del(sys.modules[deleted_module])

    singleton_instance = None

if ReloadImport.singleton_instance is None:
    ReloadImport.singleton_instance = ReloadImport()

