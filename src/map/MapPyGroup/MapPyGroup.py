"""
A group of workers which iterates through each worker in turn.
"""
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
#  group.append(MapCppSimulation())
#  group.append(MapPyRemoveTracks())
#  group.append(MapPyGlobalRecon())
#  \endcode

from types import ListType
import inspect

import ErrorHandler

class MapPyGroup:
    """
    A group of workers which iterates through each worker in turn.
    """
    def __init__(self, initial_workers = []): # pylint:disable = W0102
        """
        Constructor.
        @param self Object reference
        @param initial_workers List of 0 or more default workers.
        @param id. ID of this class.
        @throws AssertionError if initial_workers is not a list or if
        any worker therein does not satisfy the criteria of the append
        function. 
        """
        self._workers = []
        assert isinstance(initial_workers, ListType)
        for worker in initial_workers:
            self.append(worker)

    def get_worker_names(self):
        """
        Get the names of this worker and all its sub-workers.
        @param self Object reference.
        @return nested list of worker and sub-worker names e.g.
        @verbatim 
        ["MapCppTOFDigits", "MapCppTOFSlabHits", "MapCppTOFSpacePoint"]

        or

        ["MapCppTOFDigits", ["MapCppTOFSlabHits", "MapCppTOFSpacePoint"]]
        @endverbatim
        """
        names = []
        for worker in self._workers:
            if isinstance(worker, MapPyGroup):
                name = worker.get_worker_names()
            else:
                name = worker.__class__.__name__
            names.append(name)
        return names

    def append(self, worker):
        """
        Append a worker to the group.
        @param self Object reference.
        @param worker Worker.
        @throws AssertionError if the worker has no birth/2 (for
        Python) or non 0-arity birth function (for SWIG);  if the
        worker has no process/2 (for Python) or non 0-arity process
        function (for SWIG); or no death/1 function.
        """
        assert hasattr(worker, 'birth')
        py_ok = len(inspect.getargspec(worker.birth).args) == 2 # for python
        swig_ok = inspect.getargspec(worker.birth).varargs != None # for swig
        assert py_ok ^ swig_ok # exclusive or

        assert hasattr(worker, 'process')
        py_ok = len(inspect.getargspec(worker.process).args) == 2 # for python
        swig_ok = inspect.getargspec(worker.process).varargs != None # for swig
        assert py_ok ^ swig_ok # exclusive or

        assert hasattr(worker, 'death')
        assert len(inspect.getargspec(worker.death).args) == 1 # self only

        self._workers.append(worker)

    def birth(self, json_config_doc):
        """
        Birth the mapper by invoking birth on all workers.
        @param self Object reference.
        @param json_config_doc JSON configuration document.
        @return True if all workers return True else return False.
        """
        try:
            for worker in self._workers:
                assert worker.birth(json_config_doc)
        except: # pylint:disable = W0702
            ErrorHandler.HandleException({}, self)
            return False
        return True

    def process(self, spill):
        """
        Process a spill by passing it through each worker in turn,
        passing the result spill from one into the process function
        of the next.
        @param self Object reference.
        @param spill JSON spill document.
        @return result spill
        """
        nu_spill = spill
        for worker in self._workers:
            nu_spill = worker.process(nu_spill)
        return nu_spill

    def death(self):
        """
        Death the mapper by invoking death on all workers.
        @param self Object reference.
        @return True if all workers return True else return False.
        """
        try:
            for worker in self._workers:
                assert worker.death()
        except: # pylint:disable = W0702
            ErrorHandler.HandleException({}, self)
            return False
        return True

    def __del__(self):
        """
        Delete the mapper. Invoke death on all workers.
        @param self Object reference.
        """
        for worker in self._workers:
            worker.death()
