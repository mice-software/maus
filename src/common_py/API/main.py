from MapBase import MapBase
from ReduceBase import ReduceBase

class MyMap(MapBase):
    def _birth(self, config):
        self.abc = 89
    def _death(self):
        self.bob = 76
    def _process(self, data):
        #self.lkj = 98
        del self.abc


class MyReduce(ReduceBase):
    def _birth(self, config):
        self.abc = 89
    def _death(self):
        self.bob = 76
    def _process(self, data):
        self.lkj = 98
        del self.abc



m=MyMap()
m.birth('bob')
m.death()
m.process('frank')
