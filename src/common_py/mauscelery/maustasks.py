"""
MAUS Celery tasks.
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

import json
import logging

from multiprocessing import current_process 
from types import ListType
from types import StringType
from types import UnicodeType

from celery.registry import tasks
from celery.task import Task
from celery.worker.control import Panel

import MAUS
import ROOT

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

class MausGenericTransformTask(Task):
    """
    MAUS task that applies a transform. Initially the transform is
    MapPyDoNothing and the configuration is empty. Both the transform
    and configuration can be updated. 
    """
    
    def __init__(self):
        """ 
        Initialise transform to MapPyDoNothing and set PyROOT to
        batch mode to prevent canvases appearing.
        @param self Object reference.
        """
        # Set ROOT to batch mode in case certain transforms
        # use ROOT and kick up a canvas, which could give rise
        # to an exception depending on terminal window display
        # settings.
        ROOT.gROOT.SetBatch(True) # pylint: disable=E1101
        # Initial transform and JSON configuration document.
        self.__transform = MAUS.MapPyDoNothing()
        self.configuration = "{}"

    def get_transform_names(self):
        """
        Return the transform names. If the transform is MapPyGroup
        then a list of transform names is returned e.g.
        @verbatim 
        ["MapCppTOFDigits", "MapCppTOFSlabHits", "MapCppTOFSpacePoint"]
        or
        ["MapCppTOFDigits", ["MapCppTOFSlabHits", "MapCppTOFSpacePoint"]]
        @endverbatim
        @return transform name or, for MapPyGroup, list of transform names.
        """
        return WorkerUtilities.get_worker_names(self.__transform)

    def reset_task(self, configuration = None, transform = None):
        """
        Death the current transforms, create new transforms then
        birth them with the given configuration document.
        @param self Object reference.
        @param configuration JSON configuration document.
        @param transform Either a single name can be given -
        representing a single transform - or a list of transforms -
        representing a MapPyGroup. Sub-lists are treated as nested
        MapPyGroups.
        @throws ValueError If transform is not a string or a list,
        or contains an element which is not a list or string, or 
        specifies an unknown transform name; if configuration is
        not a valid JSON document.
        @throws Exception if there is a problem when death or
        birth is called.
        @throws WorkerOperationException if death or birth return
        False.
        """
        # Validate arguments before invoking death()
        if (transform != None):
            new_transform = WorkerUtilities.create_transform(transform)
        else:
            new_transform = self.__transform
        if (configuration != None):
            # Validate it's valid JSON.
            json.loads(configuration)
            self.configuration = configuration
        logger = Task.get_logger()
        if logger.isEnabledFor(logging.INFO):
            logger.info("Invoking %s.death" % self.__transform.__class__)
        if (not self.__transform.death()):
            raise WorkerDeathFailedException(self.__transform)
        self.__transform = new_transform
        if logger.isEnabledFor(logging.INFO):
            logger.info("Invoking %s.birth" % self.__transform.__class__)
        if (not self.__transform.birth(self.configuration)):
            raise WorkerBirthFailedException(self.__transform)

    def run(self, spill, client_id = "Unknown", spill_id = 0):
        """
        Apply the current transform to the spill and return the new
        spill. 
        @param self Object reference.
        @param spill JSON document string holding spill.
        @param client_id ID of client who submitted job.
        @param spill_id Index of spill from this client.
        @return JSON document string holding new spill.
        @throws Exception if there is a problem when process is called.
        """
        logger = Task.get_logger()
        if logger.isEnabledFor(logging.INFO):
            logger.info("Task invoked by %s on spill %d" \
                % (client_id, spill_id))
        spill = self.__transform.process(spill)
        return spill

tasks.register(MausGenericTransformTask) # pylint:disable=W0104, E1101

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

class CeleryWorkerUtilities: # pylint: disable=W0232
    """
    MAUS Celery worker utility methods.
    """

    @staticmethod
    def get_process_pool(panel):
        """
        Get information on the current process pool.
        @param panel Celery panel object.
        @return status document of form {master_pid: PID, master_name:
        PROCESS_NAME, pool_pids: [PID, PID, PID]}.  
        """
        doc = {}
        process_status = current_process()
        doc["master_pid"] = process_status.pid
        doc["master_name"] = process_status.name
        pool = panel.consumer.pool 
        doc["pool_pids"] = pool.info["processes"]
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.INFO):
            logger.info("Process pool: %s" % doc)
        return doc

    @staticmethod
    def restart_pool(panel):
        """
        Restart the worker pool by killing the pool processes via a
        pool.terminate_job invocation, which forces Celery to spawn
        new pool processes.  
        @param panel Celery panel object.
        @return status document of form {status:"ok"} if all went well
        or {status:"error", error:EXCEPTION_CLASS,
        message:EXCEPTION_MESSAGE} if an exception occurred.
        """
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.INFO):
            logger.info("Restarting pool")
        pool = panel.consumer.pool 
        pids = pool.info["processes"]
        doc = {}
        try:
            for pid in pids:
                if logger.isEnabledFor(logging.INFO):
                    logger.info("About to terminate %s" % pid)
                pool.terminate_job(pid)
                if logger.isEnabledFor(logging.INFO):
                    logger.info("Terminated %s" % pid)
                doc["status"] = "ok"
        except Exception as ex: # pylint:disable=W0703
            if logger.isEnabledFor(logging.ERROR):
                logger.exception(ex)
            doc["status"] = "error"
            doc["error"] = str(ex.__class__)
            doc["message"] = ex.message
        if logger.isEnabledFor(logging.INFO):
            logger.info("Status: %s" % doc)
        return doc

    @staticmethod
    def get_maus_configuration():
        """
        Get information on the current configuration and workers.
        @return status document of form {configuration:
        MAUS_CONFIGURATION_DOC, workers: WORKER_NAME_OR_LIST_OF_NAMES}
        """
        maustask = \
            tasks["mauscelery.maustasks.MausGenericTransformTask"] 
        doc = {}
        doc["configuration"] = maustask.configuration
        doc["workers"] = maustask.get_transform_names()
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.INFO):
            logger.info("Configuration: %s" % doc)
        return doc

    @staticmethod
    def set_maus_configuration(panel, configuration = None, transform = None): # pylint: disable=W0613, C0301
        """
        Death the current transforms, create new transforms then birth
        them with the given configuration document. 
        @param panel Celery panel object.
        @param configuration JSON configuration document.
        @param transform Either a single name can be given -
        representing a single transform - or a list of transforms -
        representing a MapPyGroup. Sub-lists are treated as nested
        MapPyGroups.
        @return status document of form {status:"ok"} if all went well
        or {status:"error", error:EXCEPTION_CLASS,
        message:EXCEPTION_MESSAGE} if an exception occurred.
        """
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.INFO):
            logger.info("Setting MAUS configuration")
        doc = {}
        try:
            maustask = \
                tasks["mauscelery.maustasks.MausGenericTransformTask"]
            # Configuration is unicode to decode to a normal 
            # string to avoid problems e.g. with SWIG/C++ calls.
            maustask.reset_task(configuration.encode(), transform)
            doc = {"status": "ok"}
        except Exception as ex: # pylint:disable=W0703
            if logger.isEnabledFor(logging.ERROR):
                logger.exception(ex)
            doc["status"] = "error"
            doc["error"] = str(ex.__class__)
            doc["message"] = ex.message
        if logger.isEnabledFor(logging.INFO):
            logger.info("Status: %s" % doc)
        return doc

@Panel.register
def get_process_pool(panel):
    """
    Get information on the current process pool.
    @param panel Celery panel object.
    @return status document of form {master_pid: PID, master_name:
    PROCESS_NAME, pool_pids: [PID, PID, PID]}.  
    """
    return CeleryWorkerUtilities.get_process_pool(panel)

@Panel.register
def restart_pool(panel):
    """
    Restart the worker pool by killing the pool processes via a
    pool.terminate_job invocation, which forces Celery to spawn
    new pool processes.  
    @param panel Celery panel object.
    @return status document of form {status:"ok"} if all went well
    or {status:"error", error:EXCEPTION_CLASS,
    message:EXCEPTION_MESSAGE} if an exception occurred.
    """
    return CeleryWorkerUtilities.restart_pool(panel)

@Panel.register
def get_maus_configuration(panel): # pylint:disable=W0613
    """
    Get information on the current configuration and workers.
    @param panel Celery panel object.
    @return status document of form {configuration:
    MAUS_CONFIGURATION_DOC, workers: WORKER_NAME_OR_LIST_OF_NAMES}
    """
    return CeleryWorkerUtilities.get_maus_configuration()

@Panel.register
def set_maus_configuration(panel, configuration = None, transform = None): # pylint: disable=W0613, C0301
    """
    Death the current transforms, create new transforms then birth
    them with the given configuration document. 
    @param panel Celery panel object.
    @param configuration JSON configuration document.
    @param transform Either a single name can be given -
    representing a single transform - or a list of transforms -
    representing a MapPyGroup. Sub-lists are treated as nested
    MapPyGroups.
    @return status document of form {status:"ok"} if all went well
    or {status:"error", error:EXCEPTION_CLASS,
    message:EXCEPTION_MESSAGE} if an exception occurred.
    """
    return CeleryWorkerUtilities.set_maus_configuration( \
        panel, configuration, transform)






