from IReduce import IReduce
from ModuleBase import ModuleBase

class ReduceBase(ModuleBase, IReduce):
    def process(self, data):
        return self._process(data)

    def _process(self, data):
        raise NotImplementedError('_process function not implemented in class %s' % self.__class__.__name__)
