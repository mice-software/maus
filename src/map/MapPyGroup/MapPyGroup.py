#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.


## @class MapPyGroup.MapPyGroup
#  MapPyGroup is for chaining mappers together
#
#  This class is used to chain maps together.  For example,
#  if one wants to simulate a spill then fit the spill then
#  create some accelerator physics quantity, one has to chain
#  various mappers together.  Or mathematically, if you have
#  some function/operation f(x), g(x), and h(x) on some spill
#  called 's'.  Then this allows the composition f(g(h(s))).
#
#  A demo would be the following:
#
#  \code
#  group = MapPyGroup()
#  group.Append(MapCppSimulation())
#  group.Append(MapPyRemoveTracks())
#  group.Append(MapPyGlobalRecon())
#  \endcode
#
#  @author Christopher Tunnell <c.tunnell1@physics.ox.ac.uk>

from types import ListType,InstanceType,MethodType
import inspect

class MapPyGroup:
    """Iterate through the workers

    this processor handles all the workers
    """
    def __init__(self, argWorkers=[]):
        assert isinstance(argWorkers, ListType)

        self._workers = []
        for worker in argWorkers:
            self.Append(worker)


    def append(self, argWorker):
        assert hasattr(argWorker, 'Birth')
        x = len(inspect.getargspec(argWorker.Process).args) == 2 # for python
        y = inspect.getargspec(argWorker.Process).varargs != None # for swig
        assert x ^ y # exclusive or

        assert hasattr(argWorker, 'Process')
        x = len(inspect.getargspec(argWorker.Process).args) == 2 # for python
        y = inspect.getargspec(argWorker.Process).varargs != None # for swig
        assert x ^ y # exclusive or

        assert hasattr(argWorker, 'Death')
        assert len(inspect.getargspec(argWorker.Death).args) == 1 # self only

        self._workers.append(argWorker)

    def Birth(self, argJsonConfigDocument):
        for worker in self._workers:
            worker.Birth(argJsonConfigDocument)

    def Process(self, x):
        for worker in self._workers:
            x = worker.Process(x)
        return x

    def Death(self):
        pass

    def __del__(self):
        for worker in self._workers:
            worker.Death()



if __name__ == '__main__':
    unittest.main()