from IModule import IModule

class IReduce(IModule):
    def process(self, data):
        raise NotImplementedError('process function not implemented in class %s' % self.__class__.__name__)
