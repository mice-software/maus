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

from celery.registry import tasks
from celery.task import Task
from celery.worker.control import Panel

import MAUS
import ROOT

from workers import WorkerUtilities
from workers import WorkerBirthFailedException
from workers import WorkerDeathFailedException

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

@Panel.register
def get_maus_configuration(panel): # pylint:disable=W0613
    """
    Get information on the current configuration and workers.
    @param panel Celery panel object.
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
