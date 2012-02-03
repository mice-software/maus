"""
MAUS-specific worker broadcast command handlers. Some of these handlers
invoke operations in Celery sub-processes and defined in 
mauscelery.mausprocess.

These methods are invoked by the Celery worker's main process
in response to "broadcast" calls by clients.

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
from celery.worker.control.registry import Panel 

from workers import WorkerUtilities
from mauscelery.state import MausConfiguration
from mauscelery.mausprocess import process_birth
from mauscelery.mausprocess import process_death

@Panel.register
def birth(panel, config_id, transform, configuration = "{}"): # pylint: disable=W0613, C0301
    """
    Create and birth a new transform in each sub-process. This is
    invoked by "broadcast" calls from clients and, in turn, invokes
    the process_birth method in sub-processes. Both the transform
    and configuration are validated. The configuration update request
    is only passed to sub-processes if the if the ID given by the
    client is different from the ID of the current configuration.
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
    # Only update if the configuration config_id is new.
    if (MausConfiguration.config_id != config_id):
        # List of any errors from sub-processes.
        errors = []
        try:
            # If configuration is unicode convert to a normal 
            # string to avoid problems e.g. with SWIG/C++ calls.
            if (isinstance(configuration, UnicodeType)):
                config = configuration.encode()
            else:
                config = configuration
            # Validate transform.        
            WorkerUtilities.validate_transform(transform)
            # Validate configuration.
            json.loads(configuration)
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
                status_detail = status[1]
                # Avoid duplicated information.
                if (status_detail != None) and (status_detail not in errors):
                    errors.append(status_detail)
        except Exception as exc: # pylint:disable = W0703
            status = {}
            status["error"] = str(exc.__class__)
            status["message"] = exc.message
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
    else:
        doc["status"] = "unchanged"
    if logger.isEnabledFor(logging.INFO):
        logger.info("Status: %s" % doc)
    return doc

@Panel.register
def death(panel):
    """
    Execute death on the current transform in each sub-process. This
    is invoked by broadcast calls from clients  and, in turn, invokes
    the process_death method in sub-processes.
    @param panel Celery panel object.
    @return status document of the form {"status":"ok"} if all went
    well or:{"status":"error", [{"type":ERROR,
    "message":MESSAGE},...]} if any problems arose.
    @throws Exception if any problems arose.
    """
    logger = logging.getLogger(__name__)
    if logger.isEnabledFor(logging.INFO):
        logger.info("Deathing transform")
    doc = {}
    # Get sub-process IDs from process pool.
    pool = panel.consumer.pool 
    pids = set(pool.info["processes"]) 
    pids_done = set()
    # List of any errors from sub-processes.
    errors = []
    # Submit asynchronous jobs to the sub-processes until they've all
    # processed the message.
    try:
        while pids ^ pids_done: 
            result = pool.apply_async(process_death, ())
            status = result.get()
            pids_done.add(status[0])
            status_detail = status[1]
            # Avoid duplicated information.
            if (status_detail != None) and (status_detail not in errors):
                errors.append(status_detail)
    except Exception as exc: # pylint:disable = W0703
        status = {}
        status["error"] = str(exc.__class__)
        status["message"] = exc.message
        errors.append(status)
    if (len(errors)) != 0:
        doc["status"] = "error"
        doc["error"] = errors
    else:
        doc["status"] = "ok"
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
    TRANSFORM_NAME_OR_LIST_OF_NAMES, "config_id":CONFIG_ID}
    """
    doc = {}
    doc["configuration"] = MausConfiguration.configuration
    doc["transform"] = MausConfiguration.transform
    doc["config_id"] = MausConfiguration.config_id
    return doc
