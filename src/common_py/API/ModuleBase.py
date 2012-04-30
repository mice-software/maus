from IModule import IModule

class ModuleBase(IModule):
    def birth(self, config):
        return self._birth(config)
    def death(self):
        return self._death()

    def _birth(self, config):
        raise NotImplementedError('_birth function not implemented in class %s' % self.__class__.__name__)
    def _death(self):
        raise NotImplementedError('_death function not implemented in class %s' % self.__class__.__name__)
