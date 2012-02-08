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

"""
MapPyGroup module for grouping mappers
"""

import inspect

class MapPyGroup:
    """
    @class MapPyGroup
    MapPyGroup is for chaining mappers together

    This class is used to chain maps together.  For example, if one wants to
    simulate a spill then fit the spill then create some accelerator physics 
    quantity, one has to chain various mappers together.  Or mathematically, 
    if you have some function/operation f(x), g(x), and h(x) on some spill 
    called 's'.  Then this allows the composition f(g(h(s))).
    """
    def __init__(self, arg_workers=None):
        """
        Initialise the MapPyGroup
        
        @param arg_workers list of workers to be added to the list
        """
        if arg_workers == None:
            arg_workers = []

        self._workers = []
        for worker in arg_workers:
            self.append(worker)

    def get_worker_names(self):
        """
        @return a list of the class name of each worker
        """
        names = []
        for worker in self._workers:
            names.append(worker.__class__.__name__)
        return names

    def append(self, arg_worker):
        """
        append the worker to the map

        @param arg_worker should be a mapper. MapPyGroup checks that the mapper
                has a birth, process and death function and that they have the
                appropriate call signature.
        """
        # Rogers: this should be done with inheritance!
        if not hasattr(arg_worker, 'birth'):
            raise TypeError(str(arg_worker)+' does not have a birth()')
        # python uses args, swig uses varargs
        py_arg = len(inspect.getargspec(arg_worker.birth).args) == 2
        swig_arg = inspect.getargspec(arg_worker.birth).varargs != None
        if not py_arg ^ swig_arg: # exclusive or
            raise TypeError(str(arg_worker)+' birth() has wrong call signature')

        if not hasattr(arg_worker, 'process'):
            raise TypeError(str(arg_worker)+' does not have a process()')

        # python uses args, swig uses varargs
        py_arg = len(inspect.getargspec(arg_worker.process).args) == 2
        swig_arg = inspect.getargspec(arg_worker.process).varargs != None
        if not py_arg ^ swig_arg : # exclusive or
            raise TypeError\
                         (str(arg_worker)+' process() has wrong call signature')

        if not hasattr(arg_worker, 'death'):
            raise TypeError(str(arg_worker)+' does not have a death()')
        if len(inspect.getargspec(arg_worker.death).args) != 1: # self only
            raise TypeError(str(arg_worker)+' death() has wrong call signature')

        self._workers.append(arg_worker)

    def birth(self, arg_json_config):
        """
        Call birth() on each worker. 

        @param argJsonConfigDocument string containing configuration data in
               json format

        If a worker fails to birth, will call death() on all workers (including
        the ones that have not been birthed already).

        @return True if all workers birthed correctly, else return False
        """
        for worker in self._workers:
            if not worker.birth(arg_json_config):
                print 'Failed to birth() '+worker.__class__.__name__
                for worker_death in self._workers:
                    worker_death.death()
                return False
        return True

    def process(self, spill):
        """
        Call process() on each worker. 

        @param spill string containing spill data in json format

        @return the spill document modified by all worker process calls
        """
        for worker in self._workers:
            spill = worker.process(spill)
        return spill

    def death(self):
        """
        Call death() on all workers in the group.

        @return True if all workers died successfully, else print an error and
                return False
        """
        all_dead = True
        for worker in self._workers:
            if not worker.death():
                all_dead = False
                print 'Failed to death() '+worker.__class__.__name__
        return all_dead

    def __del__(self):
        """
        Call death() on all workers
        """
        self.death()

