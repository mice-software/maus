"""
MAUS-specific sub-process command handlers.

Each Celery worker spawns one or more sub-processes to handle jobs.

Each sub-process will have a MausConfiguration and MausTransform
instance.

worker_process_init is a Celery hook. After being spawned by the
Celery worker's master process, any method registered in
worker_process_init (i.e. worker_process_init_callback) will
be invoked. This is used to set up the initial state of
the MausTransform.

The process_birth and process_death commands update the
MausConfiguration and MausTransform used by a sub-process.
They return a status method and the sub-process ID to the
caller - which is the main process. The ID is returned so
the main process knows that the sub-process has been updated. 

Any JSON configuration documents and transform names can be
assumed to be valid upon receipt.
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

import logging
import os 

from celery.signals import worker_process_init 

from mauscelery.state import MausConfiguration
from mauscelery.state import MausTransform

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

worker_process_init.connect(worker_process_init_callback) 

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
