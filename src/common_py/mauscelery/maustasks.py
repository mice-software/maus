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

import logging
import os 
import re
import signal
import sys
import traceback

from multiprocessing import current_process 
from types import ListType
from types import StringType

from celery.registry import tasks
from celery.task import Task

import MAUS
import ROOT

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
        self.__configuration = "{}"

    def create_transform(self, transform):
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
        @param self Object reference.
        @param transform Transform name or list of names.
        @return transform object or MapPyGroup object (if given a list).
        @throws ValueError if transform is not a string or a list,
        or contains an element which is not a list or string, or 
        specifies an unknown transform name.
        """
        if isinstance(transform, ListType):
            group = MAUS.MapPyGroup()
            for transform_name in transform:
                group.append(self.create_transform(transform_name))
            return group
        elif isinstance(transform, StringType):
            if not hasattr(MAUS, transform):
                raise ValueError("No such transform: %s" % transform)
            transform_class = getattr(MAUS, transform)
            return transform_class()
        else:
            raise ValueError("Transform name %s is not a string" % transform)

    def set_configuration(self, configuration):
        """
        Death the current transforms then birth them with the
        given configuration document.
        @param self Object reference.
        @param configuration JSON configuration document.
        """
        print "---set_configuration IN"
        self.__transform.death()
        # TODO add errors to return value.
        self.__configuration = configuration
        # TODO add errors to return value.
        self.__transform.birth(self.__configuration)
        # TODO add errors to return value.
        print "---set_configuration OUT"

    def set_transform(self, transform):
        """
        Death the current transforms, create new transforms then
        birth them with the current configuration document.
        @param self Object reference.
        @param transform Either a single name can be given -
        representing a single transform - or a list of transforms -
        representing a MapPyGroup. Sub-lists are treated as nested
        MapPyGroups.
        @throws ValueError if transform is not a string or a list,
        or contains an element which is not a list or string, or 
        specifies an unknown transform name.
        """
        print "---set_transform IN"
        self.__transform.death()
        # TODO add errors to return value.
        self.__transform = self.create_transform(transform)
        # TODO add errors to return value.
        self.__transform.birth(self.__configuration)
        # TODO add errors to return value.
        print "---set_transform OUT"

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
        @throws ValueError if transform is not a string or a list,
        or contains an element which is not a list or string, or 
        specifies an unknown transform name.
        """
        print "---reset_task IN"
        # Validate arguments before invoking death()
        if (transform != None):
            new_transform = self.create_transform(transform)
            # TODO add errors to return value.
        else:
            new_transform = self.__transform
        if (configuration != None):
            # TODO validate it's JSON
            self.__configuration = configuration
            # TODO add errors to return value.
        self.__transform.death()
        # TODO add errors to return value.
        self.__transform = new_transform
        self.__transform.birth(self.__configuration)
        # TODO add errors to return value.
        print "---reset_task OUT"

    def run(self, workers, spill, client_id = "Unknown", spill_id = 0):
        print "RUN!"
        print "Sleep..."
        import time
        time.sleep(20)
        print "...awake!"
        return spill

    def run2(self, spill, client_id = "Unknown", spill_id = 0):
        """
        Apply the current transform to the spill and return the new
        spill. 
        @param self Object reference.
        @param spill JSON document string holding spill.
        @param client_id ID of client who submitted job.
        @param spill_id Index of spill from this client.
        @return JSON document string holding new spill.
        """
        logger = Task.get_logger()
        if logger.isEnabledFor(logging.INFO):
            logger.info("Task invoked by %s on spill %d" \
                % (client_id, spill_id))    
            spill = self.__transform.process(spill)
        # TODO error handling.
        return spill

tasks.register(MausGenericTransformTask) # pylint:disable=W0104, E1101

from celery.worker.control import Panel

@Panel.register
def get_process_pool(panel):
    """
    Get information on the current process pool.
    @param panel Celery panel object.
    @return status document of form {master_pid: PID, master_name:
    PROCESS_NAME, pool_pids: [PID, PID, PID]}.
    """
    status = {}
    process_status = current_process()
    status["master_pid"] = process_status.pid
    status["master_name"] = process_status.name
    pool = panel.consumer.pool 
    status["pool_pids"] = pool.info["processes"]
    print "Process pool: %s" % status
    return status

@Panel.register
def reset_pool_term(panel):
    """
    Reset the worker pool by killing the pool processes via
    a TERM invocation, which forces Celery to spawn new pool
    processes. 
    @param panel Celery panel object.
    @return status document.
    """
    print "Resetting pool..."
    pool = panel.consumer.pool 
    pids = pool.info["processes"]
    status = {}
    try:
        import os 
        import signal
        for pid in pids:
            print "About to SIGTERM kill %s" % pid
            os.kill(pid, signal.SIGTERM)
            print "Killed %s" % pid
            status["status"] = "ok"
    except: # pylint: disable=W0702
        traceback.print_exc(file=sys.stdout)
        status["status"] = "error"
        # TODO add errors to return value.
    print status
    return status

@Panel.register
def reset_pool_term_job(panel):
    """
    Reset the worker pool by killing the pool processes via
    a pool.terminate_job invocation, which forces Celery to spawn new
    pool processes. 
    @param panel Celery panel object.
    @return status document.
    """
    print "Resetting pool..."
    pool = panel.consumer.pool 
    pids = pool.info["processes"]
    status = {}
    try:
        for pid in pids:
            print "About to terminate_job %s" % pid
            pool.terminate_job(pid)
            print "Terminated %s" % pid
            status["status"] = "ok"
    except: # pylint: disable=W0702
        traceback.print_exc(file=sys.stdout)
        status["status"] = "error"
        # TODO add errors to return value.
    print status
    return status

@Panel.register
def reset_pool_stopstart(panel):
    """
    Reset the worker pool by stopping and restarting the
    pool.
    @param panel Celery panel object.
    @return status document.
    """
    print "Resetting pool..."
    pool = panel.consumer.pool 
    status = {}
    try:
        pool.stop()
        pool.start()
        status["status"] = "ok"
    except: # pylint: disable=W0702
        traceback.print_exc(file = sys.stdout)
        print "Error"
        status["status"] = "error"
        # TODO add errors to status
    print status
    return status

@Panel.register
def reset_pool_resize(panel):
    pool = panel.consumer.pool
    pool_size = pool.num_processes
    print pool_size
    print "Shrinking pool..."
    pool.shrink(pool_size)
    print "Growing pool..."
    pool.grow(pool_size)
    print "Current pool size %s" % pool_size

@Panel.register
def reset_maus(panel, configuration = None, transform = None):
    """
    Death the current transforms, create new transforms then
    birth them with the given configuration document.
    @param panel Celery panel object.
    @param configuration JSON configuration document.
    @param transform Either a single name can be given -
    representing a single transform - or a list of transforms -
    representing a MapPyGroup. Sub-lists are treated as nested
    MapPyGroups.
    @throws ValueError if transform is not a list or a string
    holding a known transform class name, or if transform is a
    list but has an element which is not a list or a string
    holding a known transform class name.
    """
    print "Configuring..."
    maustask = tasks["mauscelery.maustasks.MausGenericTransformTask"]
    maustask.reset_task(configuration, transform)
    print "...configured"
    return {"status": "ok"}
