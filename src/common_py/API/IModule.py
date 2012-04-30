class IModule:
    def birth(self, config):
        raise NotImplementedError('birth function not implemented in class %s' % self.__class__.__name__)
    def death(self):
        raise NotImplementedError('death function not implemented in class %s' % self.__class__.__name__)
    
