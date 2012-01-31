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
import os 

from types import UnicodeType

from celery.task import task
from celery.task import Task
from celery.signals import worker_process_init 
from celery.worker.control import Panel
from celery.worker.control.registry import Panel 

import ROOT
from workers import WorkerUtilities
from workers import WorkerBirthFailedException
from workers import WorkerDeathFailedException
from workers import WorkerDeadException

class MausConfiguration(): # pylint:disable = W0232, R0903
    """
    MAUS transform configuration consisting of a MAUS JSON
    configuration document, a transform specification and an
    ID. The transform specification can be a single name -
    representing a single transform - or a list of transforms -
    representing a MapPyGroup. Sub-lists are treated as nested
    MapPyGroups. For example: 
    @verbatim
    ["MapCppTOFDigits", "MapCppTOFSlabHits", "MapCppTOFSpacePoint"]
    or
    ["MapCppTOFDigits", ["MapCppTOFSlabHits", "MapCppTOFSpacePoint"]]
    @endverbatim
    It is assumed that transform specification consists of valid
    transform names and that configuration is a valid JSON
    document. Callers can ensure this via calls to
    WorkerUtilities.validate_transform or json.dumps.
    This serves as a container for configuration for both the Celery
    master and sub-processes.
    """
    transform = "MapPyDoNothing"
    configuration = "{}"
    config_id = 0

class MausTransform(): # pylint:disable = W0232
    """
    MAUS transform wrapper.
    The caller must ensure that initialize is called before birth,
    process or death are used, and likewise ensure that birth is
    called before process is used. 
    This is unused by the Celery master process but each sub-process
    uses it to manage their current transform which they use to
    process client requests.
    """
    # Transform object.
    transform = None
    # Dead flag. If the transform has been death()ed this prevents
    # re-invocations of death() until birth() has been called.
    is_dead = True
        
    @classmethod
    def initialize(cls, transform_specification):
        """
        Given a transform specification, create a new transform. If
        there are existing transforms then these are death-ed first.
        @param cls Class reference.
        @param transform_specification Either a single name can be
        given - representing a single transform - or a list of
        transforms - representing a MapPyGroup. Sub-lists are treated
        as nested MapPyGroups. If None then the current transform is
        deathed and rebirthed. 
        @throw Exception if there are any problems with the
        death or initialisation.
        """
        # Death current transform.
        cls.death()
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug("Initializing transform")
            logger.debug(transform_specification)
        # Set ROOT to batch mode in case certain transforms
        # use ROOT and kick up a canvas, which could give rise
        # to an exception depending on terminal window display
        # settings.
        ROOT.gROOT.SetBatch(True) # pylint: disable=E1101
        # Create new transform.
        cls.transform = \
            WorkerUtilities.create_transform(transform_specification)

    @classmethod
    def birth(cls, configuration):
        """
        Birth the transform with the JSON configuration document. The
        transform is death-ed first if not already done.
        @param cls Class reference.
        @param configuration Valid JSON configuration document.
        @throws Exception if there is a problem when birth is called.
        @throws WorkerBirthFailedException if birth returns False.
        """
        # Death current transform.
        cls.death()
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug("Birthing transform")
            logger.debug(configuration)
        if (not cls.transform.birth(configuration)):
            raise WorkerBirthFailedException(cls.transform.__class__)
        cls.is_dead = False

    @classmethod
    def process(cls, spill):
        """
        Invoke transform.process on the spill if the transform has 
        not been death-ed.
        @param cls Class reference.
        @param spill Spill.
        @return new spill.
        @throws Exception if there is a problem.
        @throws WorkerDeadException if the worker has been death-ed.
        """
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug("Processing spill")
            logger.debug(spill)
        if not cls.is_dead:
            return cls.transform.process(spill)
        else:
            raise WorkerDeadException(cls.transform.__class__)

    @classmethod
    def death(cls):
        """
        Death the current transform if it has not been done
        already.
        @param cls Class reference.
        @throws Exception if there is a problem when death is called.
        @throws WorkerDeathFailedException if death returns False.
        """
        logger = logging.getLogger(__name__)
        if logger.isEnabledFor(logging.DEBUG):
            logger.debug("Deathing transform")
        if not cls.is_dead:
            if (not cls.transform.death()):
                raise WorkerDeathFailedException(cls.transform.__class__)
            cls.is_dead = True

def worker_process_init_callback(**kwargs): # pylint:disable = W0613
    """
    Callback from worker_process_init which is called when a Celery 
    sub-process starts. It is used to create and births a transform in
    MausTransform, using MausConfiguration. Since sub-processes will
    receive a copy of MausConfiguration from the Celery master
    process, the sub-process will always inherit the latest version of
    MausConfiguration from the master process.
    @param kwargs Arguments - unused.
    """
    logger = logging.getLogger(__name__)
    if logger.isEnabledFor(logging.INFO):
        logger.info("Creating transform %s" \
            % MausConfiguration.transform)
    MausTransform.initialize(MausConfiguration.transform)
    MausTransform.birth(MausConfiguration.configuration)

# worker_process_init is a Celery hook. Any method registered to it
# will be called when the Celery worker master process spawns a
# sub-process to handle jobs.
worker_process_init.connect(worker_process_init_callback) 

@task(name="mauscelery.maustasks.MausGenericTransformTask")
def execute_transform(spill, client_id = "Unknown", spill_id = 0):
    """
    MAUS Celery transform task used by sub-processes to execute jobs
    from clients. Proxies of this task are invoked by clients.This
    applies the current transform to the spill and returns the new
    spill.  
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
    return MausTransform.process(spill)

# The following functions manage updating of worker sub-processes,
# which execute tasks, from their main process. 

# The @Panel methods run in the main process, in response to 
# "broadcast" calls by clients. These invoke the sub-process
# process_* methods via asynchronous requests to the process_* methods
# of the sub-processes. This is based on a suggestion in
# http://groups.google.com/group/celery-developers/browse_thread/thread/191389ab86185cce # pylint:disable = C0301
# It is not the most efficient but it is the only way to ensure that 
# all sub-processes are updated at the same time and remain consistent
# with each other and the main process.

# The process_* methods are invoked in the sub-processes. These
# forward the updates to thier local MausTransform class
# and return the results along with the sub-process ID to the
# main process.

# JSON configuration documents and transforms are validated prior
# to sending to the sub-processes.

def process_birth(config_id, transform, configuration):
    """
    Create and birth a new transform. This is invoked in a sub-process
    via a call from the Celery master process. Any existing transform
    is death-ed first.
    @param config_id Configuration ID from client.
    @param transform Either a single name can be given - representing
    a single transform - or a list of transforms - representing a
    MapPyGroup. Sub-lists are treated as nested MapPyGroups. If None
    then the current transform isdeathed and rebirthed.  
    @param configuration Valid JSON configuration document.
    @return status of (PID, {"status":"ok"}) if all went well,
    (PID, {"status":"error", "type":ERROR, "message":MESSAGE}) if
    an exception arose or (PID, None) if the birth was not done as the
    sub-process MausConfiguration.config_id matches config_id.
    """
    # Only update if the configuration config_id is new.
    if (MausConfiguration.config_id != config_id):
        doc = {}
        try:
            logger = logging.getLogger(__name__)
            if logger.isEnabledFor(logging.INFO):
                logger.info("Birthing transform %s" % transform)
            MausTransform.initialize(transform)
            MausTransform.birth(configuration)
            # Update sub-process configuration.
            MausConfiguration.configuration = configuration
            MausConfiguration.transform = transform
            MausConfiguration.config_id = config_id
            doc["status"] = "ok"
        except Exception as exc: # pylint:disable = W0703
            if logger.isEnabledFor(logging.DEBUG):
                logger.debug(exc)
            doc["status"] = "error"
            doc["type"] = str(exc.__class__)
            doc["message"] = exc.message
        return (os.getpid(), doc)
    else:
        return (os.getpid(), None)

def process_death():
    """
    Execute death on the current transform. This is invoked in a
    sub-process via a call from the Celery master process.
    @return sub-process ID to indicate that this process has executed
    this method.
    @return status of (PID, {"status":"ok"}) if all went well,
    (PID, {"status":"error", "type":ERROR, "message":MESSAGE}) if
    an exception arose or (PID, None) if death has already been
    invoked.
    """
    # Only update if transform is not already dead.
    if (not MausTransform.is_dead):
        doc = {}
        try:
            logger = logging.getLogger(__name__)
            if logger.isEnabledFor(logging.INFO):
                logger.info("Deathing transform")
            MausTransform.death() 
            doc["status"] = "ok"
        except Exception as exc: # pylint:disable = W0703
            if logger.isEnabledFor(logging.DEBUG):
                logger.debug(exc)
            doc["status"] = "error"
            doc["type"] = str(exc.__class__)
            doc["message"] = exc.message
        return (os.getpid(), doc)
    else:
        return (os.getpid(), None)

@Panel.register
def birth(panel, config_id, transform, configuration = "{}"): # pylint: disable=W0613, C0301
    """
    Create and birth a new transform in each sub-process. This is
    invoked by "broadcast" calls from clients and, in turn, invokes
    the process_birth method in sub-processes. Both the transform
    and configuration are validated. The configuration is updated
    only if the ID given by the client is different from the 
    ID of the current configuration.
    @param panel Celery panel object.
    @param config_id Configuration ID from client.
    @param transform Either a single name can be given - representing
    a single transform - or a list of transforms - representing a
    MapPyGroup. Sub-lists are treated as nested MapPyGroups. If None
    then the current transform isdeathed and rebirthed.  
    @param configuration JSON configuration document.
    @return status document with entries of form PID:{"status":"ok"}
    if all went well and PID:{"status":"error", "type":ERROR,
    "message":MESSAGE} if things went wrong in updating a process.
    @throws Exception if any problems arise in communicating with
    sub-processes.
    """
    logger = logging.getLogger(__name__)
    if logger.isEnabledFor(logging.INFO):
        logger.info("Birthing transform %s" % transform)
    doc = {}
    # If configuration is unicode convert to a normal 
    # string to avoid problems e.g. with SWIG/C++ calls.
    if (isinstance(configuration, UnicodeType)):
        config = configuration.encode()
    else:
        config = configuration
    # Validate transform.        
    WorkerUtilities.validate_transform(transform)
    # Validate configuration.
    json.dumps(configuration)
    # Get sub-process IDs from process pool.
    pool = panel.consumer.pool 
    pids = set(pool.info["processes"]) 
    pids_done = set() 
    # Submit asynchronous jobs to the sub-processes until they've all
    # processed the message.
    while pids ^ pids_done: 
        result = pool.apply_async( \
            process_birth, (config_id, transform, config,))
        status = result.get()
        pids_done.add(status[0])
        if status[1] != None:
            doc[str(status[0])] = status[1]
    # Update master process configuration.
    MausConfiguration.config_id = config_id
    MausConfiguration.configuration = config
    MausConfiguration.transform = transform
    if logger.isEnabledFor(logging.INFO):
        logger.info("Status: %s" % doc)
    return doc

@Panel.register
def death(panel):
    """
    Execute death on the current transform in each sub-process. This
    is invoked by "broadcast" calls from clients  and, in turn,
    invokes the process_death method in sub-processes.  
    @param panel Celery panel object.
    @return status document with entries of form PID:{"status":"ok"}
    if all went well and PID:{"status":"error", "type":ERROR,
    "message":MESSAGE} if things went wrong in updating a process.
    @throws Exception if any problems arise in communicating with
    sub-processes.
    """
    logger = logging.getLogger(__name__)
    if logger.isEnabledFor(logging.INFO):
        logger.info("Deathing transform")
    doc = {}
    # Get sub-process IDs from process pool.
    pool = panel.consumer.pool 
    pids = set(pool.info["processes"]) 
    pids_done = set() 
    # Submit asynchronous jobs to the sub-processes until they've all
    # processed the message.
    while pids ^ pids_done: 
        result = pool.apply_async(process_death, ())
        status = result.get()
        pids_done.add(status[0])
        if status[1] != None:
            doc[str(status[0])] = status[1]
    if logger.isEnabledFor(logging.INFO):
        logger.info("Status: %s" % doc)
    return doc

@Panel.register
def get_maus_configuration(panel): # pylint:disable=W0613
    """
    Get information on the current configuration and transform.
    @param panel Celery panel object.
    @return status document of form {configuration:
    MAUS_CONFIGURATION_DOC, transform:
    TRANSFORM_NAME_OR_LIST_OF_NAMES, config_id:CONFIG_ID}
    """
    doc = {}
    doc["configuration"] = MausConfiguration.configuration
    doc["transform"] = MausConfiguration.transform
    doc["config_id"] = MausConfiguration.config_id
    return doc
