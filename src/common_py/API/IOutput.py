from IModule import IModule

class IOutput(IModule):
    def save(self, data):
        raise NotImplementedError('save function not implemented in class %s' % self.__class__.__name__)
