"""
MAUS-specific worker main process commands. Some of these are
"broadcast" handlers and invoke operations in Celery sub-processes and
defined in mauscelery.mausprocess.

Each Celery worker spawns one or more sub-processes to handle
jobs. Each sub-process will have a MausConfiguration and MausTransform
instance.

These methods invoke sub-process methods via asynchronous requests to
the those methods. This is based on a suggestion in 
http://groups.google.com/group/celery-developers/browse_thread/
thread/191389ab86185cce
which repeatedly sends the same request to all the sub-processes until
all confirm that they have executed the request. This is done via
their returning of a status message and their sub-process ID.

It is not the most efficient but it is the only way to ensure that
all sub-processes are updated at the same time and remain consistent 
with both each other and the main process.

JSON configuration documents and transforms are validated prior to
sending to the sub-processes. 
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
from types import UnicodeType

from celery.worker.control import Panel

from framework.workers import WorkerUtilities
from mauscelery.state import MausConfiguration
from mauscelery.mausprocess import process_birth
from mauscelery.mausprocess import process_death

from Configuration import Configuration

from celery.signals import worker_init 

def worker_init_callback(**kwargs): # pylint:disable = W0613
    """
    Callback from worker_init which is called when the Celery main
    worker process starts. It is used to read the MAUS configuration
    and extract the current MAUS version from this and put it into
    MausConfiguration. Since sub-processes will receive a copy of
    MausConfiguration from the Celery master process, the sub-process
    will always inherit the latest version of MausConfiguration from
    the master process. 
    @param kwargs Arguments - unused.
    """
    configuration  = Configuration()
    config_doc = configuration.getConfigJSON()
    config_dictionary = json.loads(config_doc)
    MausConfiguration.version = config_dictionary["maus_version"]
    logger = logging.getLogger(__name__)
    if logger.isEnabledFor(logging.INFO):
        logger.info("MAUS version: %s" % MausConfiguration.version)

# Bind the callback method to the Celery worker_init signal.
worker_init.connect(worker_init_callback) 

def sub_process_broadcast(panel, func, arguments, errors):
    """
    Invoke a function on every sub-process via asynchronous requests.   
    This is based on a suggestion in 
    http://groups.google.com/group/celery-developers/browse_thread/
    thread/191389ab86185cce
    which repeatedly sends the same request to all the sub-processes
    until all confirm that they have executed the request. This is
    done via their returning of a status message and their sub-process
    ID. It is not the most efficient but it is the only way to ensure
    that all sub-processes are updated at the same time and remain
    consistent with both each other and the main process.
    @param panel Celery panel object.
    @param func Function to invoke.
    @param arguments Arguments to pass to function.
    @param errors List to which error information is to be appended. 
    @throws Exception if any problems arise in communicating with
    sub-processes.
    """
    # Get sub-process IDs from process pool.
    pool = panel.consumer.pool 
    pids = set(pool.info["processes"]) 
    pids_done = set() 
    # Create new argument list with the current set of process IDs.
    # PIDs are passed to sub-processes to ensure they only call
    # their function once.
    pids_arguments = (pids_done,) + arguments
    # Submit asynchronous jobs to the sub-processes until they've all
    # processed the message.
    data = []
    while pids ^ pids_done: 
        result = pool.apply_async(func, pids_arguments)
        status = result.get()
        pids_done.add(status[0])
        status_detail = status[1]
        if status[2] != '':
            data.append(status[2])
        # Avoid duplicated information.
        if (status_detail != None) and (status_detail not in errors):
            errors.append(status_detail)
    return data

@Panel.register
def birth(panel, config_id, transform, run_number, configuration = "{}"): # pylint: disable=W0613, C0301
    """
    Create and birth a new transform in each sub-process. This is
    invoked by "broadcast" calls from clients and, in turn, invokes
    the process_birth method in sub-processes. Both the transform
    and configuration are validated. 
    @param panel Celery panel object.
    @param config_id Configuration ID from client.
    @param transform Either a single name can be given - representing
    a single transform - or a list of transforms - representing a
    MapPyGroup. Sub-lists are treated as nested MapPyGroups. If None
    then the current transform isdeathed and rebirthed.  
    @param configuration JSON configuration document.
    @return status document with entries of form {"status":"ok"}
    if all went well and {"status":"error", "error":[{"message":"...",
    "type":"TYPE"},...]} if things went wrong.
    """
    logger = logging.getLogger(__name__)
    if logger.isEnabledFor(logging.INFO):
        logger.info("Birthing transform %s" % transform)
    doc = {}
    # List of any errors from sub-processes.
    errors = []
    run_header = ""
    try:
        # If configuration is unicode convert to a normal 
        # string to avoid problems e.g. with SWIG/C++ calls.
        if (isinstance(configuration, UnicodeType)):
            config = configuration.encode()
        else:
            config = configuration
        # Validate transform.        
        WorkerUtilities.validate_transform(transform)
        # Validate configuration is valid JSON.
        config_doc = json.loads(configuration)
        # Check MAUS version number is consistent.
        if (config_doc["maus_version"] != MausConfiguration.version):
            raise ValueError("maus_version: expected %s, got %s" % 
                (MausConfiguration.version, config_doc["maus_version"]))
        # Invoke process_birth on all sub-processes
        run_header = sub_process_broadcast(panel, process_birth, 
            (config_id, transform, config, run_number), errors)
    except Exception as exc: # pylint:disable = W0703
        status = {}
        status["error"] = str(exc.__class__)
        status["message"] = str(exc)
        errors.append(status)
    if (len(errors)) != 0:
        doc["status"] = "error"
        doc["error"] = errors
    else:
        doc["status"] = "ok"
        # Update master process configuration.
        MausConfiguration.config_id = config_id
        MausConfiguration.configuration = config
        MausConfiguration.transform = transform
    doc["run_headers"] = run_header
    if logger.isEnabledFor(logging.INFO):
        logger.info("Status: %s" % doc)
    return doc

@Panel.register
def death(panel, run_number):
    """
    Execute death on the current transform in each sub-process. This
    is invoked by broadcast calls from clients  and, in turn, invokes
    the process_death method in sub-processes.
    @param panel Celery panel object.
    @return status document with entries of form {"status":"ok"}
    if all went well and {"status":"error", "error":[{"message":"...",
    "type":"TYPE"},...]} if things went wrong.
    """
    logger = logging.getLogger(__name__)
    if logger.isEnabledFor(logging.INFO):
        logger.info("Deathing transform")
    doc = {}
    # List of any errors from sub-processes.
    errors = []
    run_footer = ""
    # Submit asynchronous jobs to the sub-processes until they've all
    # processed the message.
    try:
        # Invoke process_death on all sub-processes
        run_footer = sub_process_broadcast(panel, process_death, (run_number,),
                                           errors)
    except Exception as exc: # pylint:disable = W0703
        status = {}
        status["error"] = str(exc.__class__)
        status["message"] = str(exc)
        errors.append(status)
    if (len(errors)) != 0:
        doc["status"] = "error"
        doc["error"] = errors
    else:
        doc["status"] = "ok"
    doc["run_footers"] = run_footer
    if logger.isEnabledFor(logging.INFO):
        logger.info("Status: %s" % doc)
    return doc

@Panel.register
def get_maus_configuration(panel): # pylint:disable=W0613
    """
    Get information on the current configuration and transform.
    @param panel Celery panel object.
    @return status document of form {"configuration":
    MAUS_CONFIGURATION_DOC, "transform":
    TRANSFORM_NAME_OR_LIST_OF_NAMES, "config_id":CONFIG_ID,
    "version":MAUS_VERSION}
    """
    doc = {}
    doc["configuration"] = MausConfiguration.configuration
    doc["transform"] = MausConfiguration.transform
    doc["config_id"] = MausConfiguration.config_id
    doc["version"] = MausConfiguration.version
    return doc
