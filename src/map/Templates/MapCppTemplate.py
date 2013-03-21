import _MapCppTemplate

class MapCppTemplate():
    """ Automatically generated wrapper for _MapCppTemplate"""
    def birth(self, *args):   return _MapCppTemplate.birth(self.this, *args)
    def death(self):          return _MapCppTemplate.death(self.this)
    def process(self, *args): return _MapCppTemplate.process(self.this, *args)
    def __init__(self):
        print "__init__ called here"
        this = _MapCppTemplate.module_new()
        try: self.this.append(this)
        except: self.this = this
    def __del__(self):
        print "__del__ called here"
        _MapCppTemplate.module_delete(self.this)
    def CanConvert(self): return True
