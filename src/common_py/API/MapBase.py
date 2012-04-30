from IMap import IMap
from ModuleBase import ModuleBase
from Constable import const

class MapBase(ModuleBase, IMap):
    @const
    def process(self, data):
        return self._process(data)

    def _process(self, data):
        raise NotImplementedError('_process function not implemented in class %s' % self.__class__.__name__)
