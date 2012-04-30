from IModule import IModule
from Constable import const

class IMap(IModule):  
    @const
    def process(self, data):
        raise NotImplementedError('process function not implemented in class %s' % self.__class__.__name__)
