from IModule import IModule

class IInput(IModule):
    def emitter(self):
        raise NotImplementedError('emitter function not implemented in class %s' % self.__class__.__name__)
