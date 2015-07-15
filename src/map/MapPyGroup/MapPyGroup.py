"""
A group of workers which iterates through each worker in turn.
"""
#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

from types import ListType
import sys

import ErrorHandler
from maus_cpp import converter

class MapPyGroup:
    """
    @class MapPyGroup
    MapPyGroup is for chaining workers together

    This class is used to chain maps together.  For example, if one
    wants to simulate a spill then fit the spill then create some
    accelerator physics quantity, one has to chain various maps
    together. Or mathematically, if you have some function/operation
    f(x), g(x), and h(x) on some spill called 's'.  Then this allows
    the composition f(g(h(s))). 
    """

    def __init__(self, initial_workers = []): # pylint:disable = W0102
        """
        Constructor.
        @param self Object reference
        @param initial_workers List of 0 or more default workers.
        @param id. ID of this class.
        @throws TypeError if initial_workers is not a list or if
        any worker therein does not satisfy the criteria of the append
        function. 
        """
        self._workers = []
        if (not isinstance(initial_workers, ListType)):
            raise TypeError("Initial workers must be a list")
        for worker in initial_workers:
            self.append(worker)

    def get_worker_names(self):
        """
        Get the names of the group's members, recursing into
        any nested groups. 
        @param self Object reference.
        @return nested list of member names e.g.
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
        @param worker Worker which should have birth, process and
        death functions with the appropriate call signatures.

        @throws TypeError if the worker has no birth/2 (for
        Python) or non 0-arity birth function (for SWIG);  if the
        worker has no process/2 (for Python) or non 0-arity process
        function (for SWIG); or no death/1 function.
        """
        if not hasattr(worker, 'birth'):
            raise TypeError(str(worker) + ' does not have a birth()')
        if not hasattr(worker, 'process'):
            raise TypeError(str(worker) + ' does not have a process()')
        if not hasattr(worker, 'death'):
            raise TypeError(str(worker)+' does not have a death()')
        self._workers.append(worker)

    def birth(self, json_config_doc):
        """
        Birth the group by invoking birth on all members. If a member
        fails to birth then already-birthed members will be deathed.
        @param self Object reference.
        @param json_config_doc JSON configuration document.
        @return True if all members return True else return False.
        @throws MapPyGroupBirthException if a member's birth
        function threw an exception or a member's death function 
        raised an exception during clean-up.
        """
        exceptions = [] # List of exceptions from each member.
        birth_ok = True
        for worker in self._workers:
            try:
                # Because None is also considered a pass, we can't
                # simply use 'and' here.
                worker_birth_ok = worker.birth(json_config_doc)
                if (worker_birth_ok is None):
                    worker_birth_ok = True
                birth_ok = birth_ok and worker_birth_ok
            except: # pylint:disable = W0702
                # Record the exception.
                new_exc = str(sys.exc_info()[0])+": " +\
                          str(sys.exc_info()[1])
                sys.excepthook(*sys.exc_info())
                exceptions.append(worker.__class__.__name__+":"+new_exc)
                birth_ok = False
            if (not birth_ok):
                # Break out the loop now.
                break
        if (not birth_ok):
            # Death the members.
            try:
                self.death()
            except: # pylint:disable = W0702
                exceptions.append(str(sys.exc_info()[0]) + ": " + 
                    str(sys.exc_info()[1]))
            if (len(exceptions) != 0):
                try:
                    raise MapPyGroupBirthException(exceptions)
                except: # pylint:disable = W0702
                    # ErrorHandler has the final say as to whether
                    # an exception is thrown.
                    ErrorHandler.HandleException({}, self)
            return False
        return True

    def process(self, spill):
        """
        Process a spill by passing it through each member in turn,
        passing the result spill from one into the process function
        of the next.
        @param self Object reference.
        @param spill JSON spill document.
        @return result spill modified by the group members.
        """
        nu_spill = spill
        for worker in self._workers:
            if not (hasattr(worker, "can_convert") and worker.can_convert):
                old_spill = converter.string_repr(nu_spill)
                try:
                    converter.del_data_repr(nu_spill)
                except TypeError:
                    pass
                del nu_spill # should be no references to nu_spill left
            else:
                old_spill = nu_spill
                del nu_spill # should be no references to nu_spill left
            nu_spill = worker.process(old_spill)
            try:
                converter.del_data_repr(old_spill)
            except TypeError:
                pass
            del old_spill # should be no references to old_spill left
        return nu_spill

    def death(self):
        """
        Death the group by invoking death on all members.
        @param self Object reference.
        @return True if all members return True else return False.
        @throws MapPyGroupDeathException if any member's death
        function threw an exception and the ErrorHandler is set to
        raise exceptions.
        """
        death_ok = True
        exceptions = [] # List of exceptions from each member.
        for worker in self._workers:
            try:
                # Because None is also considered a pass, we can't
                # simply use 'and' here.
                worker_death_ok = worker.death()
                if (worker_death_ok is None):
                    worker_death_ok = True
                death_ok = death_ok and worker_death_ok
            except: # pylint:disable = W0702
                # Record the exception and continue.
                exceptions.append(worker.__class__.__name__ + ": " + 
                    str(sys.exc_info()[0]) + ": " + 
                    str(sys.exc_info()[1]))
                death_ok = False
        if (not death_ok):
            if (len(exceptions) != 0):
                try:
                    raise MapPyGroupDeathException(exceptions)
                except: # pylint:disable = W0702
                    # ErrorHandler has the final say as to whether
                    # an exception is thrown.
                    ErrorHandler.HandleException({}, self)
            return False
        return True

    def __del__(self):
        """
        Delete the mapper. Invoke death on all members.
        @param self Object reference.
        """
        self.death()

class MapPyGroupException(Exception):
    """ 
    Exception raised if member of a MapPyGroup throws an
    exception. 
    """

    def __init__(self, exceptions):
        """
        Constructor. Overrides Exception.__init__.
        @param self Object reference.
        @param exceptions Information on exceptions from members.
        """
        Exception.__init__(self)
        self._exceptions = exceptions

    def __str__(self):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        return "\n".join(self._exceptions)

class MapPyGroupBirthException(MapPyGroupException):
    """ 
    Exception raised if member of a MapPyGroup throws an
    exception during birth.
    """

class MapPyGroupDeathException(MapPyGroupException):
    """ 
    Exception raised if member of a MapPyGroup throws an
    exception during death.
    """
