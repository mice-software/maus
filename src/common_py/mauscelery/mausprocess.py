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

import ErrorHandler

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
        logger.info("Setting MAUS ErrorHandler to raise exceptions")
    ErrorHandler.DefaultHandler().on_error = 'raise'
    MausTransform.initialize(MausConfiguration.transform)
    MausTransform.birth(MausConfiguration.configuration)

# Bind the callback method to the Celery worker_process_init signal.
worker_process_init.connect(worker_process_init_callback) 

def process_birth(pids, config_id, transform, configuration):
    """
    Create and birth a new transform. This is invoked in a sub-process
    via a call from the Celery master process. Any existing transform
    is death-ed first. 
    @aram pids List of process IDs whose process_birth method has been
    invoked. If this process is in the list then this method just returns
    (PID, None).
    @param config_id Configuration ID from client.
    @param transform Either a single name can be given - representing
    a single transform - or a list of transforms - representing a
    MapPyGroup. Sub-lists are treated as nested MapPyGroups. If None
    then the current transform is deathed and rebirthed.  
    @param configuration Valid JSON configuration document.
    @return status of (PID, None) if all went well or (PID,
    {"error":ERROR, "message":MESSAGE}) if an exception arose. PID is
    the sub-process ID. This lets the master process know that the
    sub-process has executed this operation. 
    """
    status = None
    logger = logging.getLogger(__name__)
    # Check if processed already.  
    if (not os.getpid() in pids):
        try:
            if logger.isEnabledFor(logging.INFO):
                logger.info("Birthing transform %s" % transform)
            MausTransform.initialize(transform)
            MausTransform.birth(configuration)
            # Update sub-process configuration.
            MausConfiguration.configuration = configuration
            MausConfiguration.transform = transform
            MausConfiguration.config_id = config_id
        except Exception as exc: # pylint:disable = W0703
            status = {}
            status["error"] = str(exc.__class__)
            status["message"] = str(exc)
    if logger.isEnabledFor(logging.DEBUG):
        logger.debug("Status: %s " % status)
    return (os.getpid(), status)

def process_death(pids):
    """
    Execute death on the current transform. This is invoked in a
    sub-process via a call from the Celery master process. If death
    has already been invoked then this is a no-op. 
    @aram pids List of process IDs whose process_birth method has been
    invoked. If this process is in the list then this method just returns
    (PID, None).
    @return status of (PID, None) if all went well or (PID,
    {"error":ERROR, "message":MESSAGE}) if an exception arose. PID is
    the sub-process ID. This lets the master process know that the
    sub-process has executed this operation. 
    """
    status = None
    logger = logging.getLogger(__name__)
    # Check if processed already.
    if (not os.getpid() in pids):
        # Only call if the transform is not already dead.
        if (not MausTransform.is_dead):
            try:
                if logger.isEnabledFor(logging.INFO):
                    logger.info("Deathing transform")
                MausTransform.death()
            except Exception as exc: # pylint:disable = W0703
                status = {}
                status["error"] = str(exc.__class__)
                status["message"] = str(exc)
    if logger.isEnabledFor(logging.DEBUG):
        logger.debug("Status: %s " % status)
    return (os.getpid(), status)
