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

import MAUS

from types import ListType
from types import StringType
from types import UnicodeType

from MapPyGroup import MapPyGroup

class WorkerUtilities: # pylint: disable=W0232
    """
    MAUS worker utility methods.
    """

    @classmethod
    def create_transform(cls, transform):
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
        Transforms must be in modules named after the transform class
        e.g. for the above the transform MapCppTOFSlabHits must be 
        in a class MapCppTOFSlabHits.MapCppTOFSlabHits.
        @param cls Class reference.
        @param transform Transform name or list of names.
        @return transform object or MapPyGroup object (if given a list).
        @throws ValueError if transform is not a string or a list,
        or contains an element which is not a list or string, or 
        specifies an unknown transform name.
        """
        if isinstance(transform, ListType):
            group = MapPyGroup()
            for transform_name in transform:
                group.append(cls.create_transform(transform_name))
            return group
        elif isinstance(transform, StringType) \
            or isinstance(transform, UnicodeType):
            transform_class = getattr(MAUS, transform)
            return transform_class()
        else:
            raise ValueError("Transform name %s is not a string" % transform)

    @classmethod
    def validate_transform(cls, transform):
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
        Transforms must be in modules named after the transform class
        e.g. for the above the transform MapCppTOFSlabHits must be 
        in a class MapCppTOFSlabHits.MapCppTOFSlabHits.
        @param cls Class reference.
        @param transform Transform name or list of names.
        @throws ValueError if transform is not a string or a list,
        or contains an element which is not a list or string, or 
        specifies an unknown transform name.
        """
        if isinstance(transform, ListType):
            for transform_name in transform:
                cls.validate_transform(transform_name)
        elif isinstance(transform, StringType) \
            or isinstance(transform, UnicodeType):
            try:
                getattr(MAUS, transform)
            except AttributeError: 
                raise ValueError("No such transform: %s" % transform)
        else:
            raise ValueError("Transform name %s is not a string" % transform)

    @classmethod
    def get_worker_names(cls, worker):
        """
        Given a worker class get the name of the worker. If the
        worker is MapPyGroup then a list of worker names is
        returned e.g.
        @verbatim 
        ["MapCppTOFDigits", "MapCppTOFSlabHits", "MapCppTOFSpacePoint"]
        or
        ["MapCppTOFDigits", ["MapCppTOFSlabHits", "MapCppTOFSpacePoint"]]
        @endverbatim
        @param cls Class reference.
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
        self._worker = worker

class WorkerBirthFailedException(WorkerOperationException):
    """ Exception raised if MAUS worker birth returns False. """

    def __str__(self):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        return "%s returned False" % self._worker

class WorkerDeathFailedException(WorkerOperationException):
    """ Exception raised if MAUS worker death returns False. """

    def __str__(self):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        return "%s returned False" % self._worker

class WorkerDeadException(WorkerOperationException):
    """ 
    Exception raised if MAUS worker process is called but the worker
    is dead.
    """

    def __str__(self):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        return "%s process called after death" % self._worker

class WorkerProcessException(WorkerOperationException):
    """ 
    Exception raised if MAUS worker process throws an exception.
    """

    def __init__(self, worker, details = None):
        """
        Constructor. Overrides WorkerOperationException.__init__.
        @param self Object reference.
        @param worker Name of worker that failed.
        @param details Details on the exception.
        """
        WorkerOperationException.__init__(self, worker)
        self._details = details

    def __str__(self):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        if (self._details == None):
            detail_str = ""
        else:
            detail_str = ": %s" % self._details
        return "%s process threw an exception%s" \
            % (self._worker, detail_str)
