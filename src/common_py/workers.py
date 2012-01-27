"""
MAUS worker utilities.
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

from types import ListType
from types import StringType
from types import UnicodeType

import MAUS

class WorkerUtilities: # pylint: disable=W0232
    """
    MAUS worker utility methods.
    """

    @staticmethod
    def create_transform(transform):
        """
        Create a transform given the name of transform
        class(es). Either a single name can be given - representing a
        single transform - or a list of transforms - representing a 
        MapPyGroup. Sub-lists are treated as nested
        MapPyGroups. Example list arguments include:  
        @verbatim 
        []
        ["MapCppTOFDigits", "MapCppTOFSlabHits", "MapCppTOFSpacePoint"]
        ["MapCppTOFDigits", ["MapCppTOFSlabHits", "MapCppTOFSpacePoint"]]
        @endverbatim
        Transforms must be in the MAUS module namespace e.g. 
        MAUS.MapCppTOFSlabHits.
        @param transform Transform name or list of names.
        @return transform object or MapPyGroup object (if given a list).
        @throws ValueError if transform is not a string or a list,
        or contains an element which is not a list or string, or 
        specifies an unknown transform name.
        """
        if isinstance(transform, ListType):
            group = MAUS.MapPyGroup()
            for transform_name in transform:
                group.append( \
                    WorkerUtilities.create_transform(transform_name))
            return group
        elif isinstance(transform, StringType) \
            or isinstance(transform, UnicodeType):
            if not hasattr(MAUS, transform):
                raise ValueError("No such transform: %s" % transform)
            transform_class = getattr(MAUS, transform)
            return transform_class()
        else:
            raise ValueError("Transform name %s is not a string" % transform)

    @staticmethod
    def validate_transform(transform):
        """
        Validate whether the names of transform class(es) are valid
        transforms. Either a single name can be given - representing a
        single transform - or a list of transforms - representing a 
        MapPyGroup. Sub-lists are treated as nested
        MapPyGroups. Example list arguments include:  
        @verbatim 
        []
        ["MapCppTOFDigits", "MapCppTOFSlabHits", "MapCppTOFSpacePoint"]
        ["MapCppTOFDigits", ["MapCppTOFSlabHits", "MapCppTOFSpacePoint"]]
        @endverbatim
        Transforms must be in the MAUS module namespace e.g. 
        MAUS.MapCppTOFSlabHits.
        @param transform Transform name or list of names.
        @throws ValueError if transform is not a string or a list,
        or contains an element which is not a list or string, or 
        specifies an unknown transform name.
        """
        if isinstance(transform, ListType):
            for transform_name in transform:
                WorkerUtilities.validate_transform(transform_name)
        elif isinstance(transform, StringType) \
            or isinstance(transform, UnicodeType):
            if not hasattr(MAUS, transform):
                raise ValueError("No such transform: %s" % transform)
        else:
            raise ValueError("Transform name %s is not a string" % transform)

    @staticmethod
    def get_worker_names(worker):
        """
        Given a worker class get the name of the worker. If the
        worker is MapPyGroup then a list of worker names is
        returned e.g.
        @verbatim 
        ["MapCppTOFDigits", "MapCppTOFSlabHits", "MapCppTOFSpacePoint"]
        or
        ["MapCppTOFDigits", ["MapCppTOFSlabHits", "MapCppTOFSpacePoint"]]
        @endverbatim
        @param worker Worker.
        @return worker name or, for MapPyGroup, list of worker names.
        """
        if hasattr(worker, "get_worker_names"):
            workers = worker.get_worker_names()
        else:
            workers = worker.__class__.__name__
        return workers

class WorkerOperationException(Exception):
    """ Exception raised if a MAUS worker operation returns False. """

    def __init__(self, worker):
        """
        Constructor. Overrides Exception.__init__.
        @param self Object reference.
        @param worker Name of worker that failed.
        """
        Exception.__init__(self)
        self.__worker = worker

    def __str__(self):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        return "%s returned False" % self.__worker

class WorkerBirthFailedException(WorkerOperationException):
    """ Exception raised if MAUS worker birth returns False. """

class WorkerDeathFailedException(WorkerOperationException):
    """ Exception raised if MAUS worker death returns False. """

class WorkerDeadException(WorkerOperationException):
    """ 
    Exception raised if MAUS worker process is called but the worker
    is dead.
    """
