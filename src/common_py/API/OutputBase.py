from IOutput import IOutput
from ModuleBase import ModuleBase

class OutputBase(ModuleBase, IOutput):
    def save(self, data):
        return self._save(data)

    def _save(self, data):
        raise NotImplementedError('_save function not implemented in class %s' % self.__class__.__name__)
