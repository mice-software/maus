from IInput import IInput
from ModuleBase import ModuleBase

class InputBase(ModuleBase, IInput):
    def emitter(self):
        return self._emitter()

    def _emitter(self):
        raise NotImplementedError('_emitter function not implemented in class %s' % self.__class__.__name__)
