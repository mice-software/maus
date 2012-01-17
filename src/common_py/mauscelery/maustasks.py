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
import re
from types import ListType
from types import StringType

from celery.registry import tasks
from celery.task import Task

import MAUS
import ROOT

class MausGenericTransformTask(Task):
    """
    MAUS task that creates an instance of each type of map worker
    then uses these to execute pipelines specified by clients.
    """
    
    def __init__(self):
        """ 
        Initialise transform to None.
        @param self Object reference.
        """
        # Set ROOT to batch mode as certain mappers e.g. MapPyTOFPlot
        # otherwise kick up a PyROOT canvas.
        ROOT.gROOT.SetBatch(True) # pylint: disable=E1101
        self.__transforms = {}
        for name in dir(MAUS):
            if re.match("Map", name):
                map_class = getattr(MAUS, name)
                self.__transforms[name] = map_class()
        self.__config = 0

    def configure(self, config):
        self.__config = config

    def birth(self, json_config_doc):
        """
        Setup transform then invoke birth with the JSON
        configuration document.
        @param self Object reference.
        @param json_config_doc JSON configuration document.       
        @return result of invoking birth.
        @throws exception if any exceptions arise in birth().
        """
        for name in self.__transforms:
#            self.__transforms[name].birth(json_config_doc)

            # MapCppSimulation takes ages to initialise so skip for now.
            if name == "MapCppSimulation":
                continue
            self.__transforms[name].birth(json_config_doc)
        self.__config = self.__config + 1
        return True

    def run(self, workers, spill, client_id = "Unknown", spill_id = 0):
        """
        Apply the transform to the spill and return the new spill.
        @param self Object reference.
        @param workers Nested list of names of workers to invoke
        when processing the spill e.g.
        @verbatim 
        ["MapCppTOFDigits", "MapCppTOFSlabHits", "MapCppTOFSpacePoint"]
        or
        ["MapCppTOFDigits", ["MapCppTOFSlabHits", "MapCppTOFSpacePoint"]]
        @endverbatim
        @param spill JSON document string holding spill.
        @param client_id ID of client who submitted job.
        @param spill_id Index of spill from this client.
        @return JSON document string holding new spill.
        """
        print "Configuration: %s" % self.__config
        logger = Task.get_logger()
        if logger.isEnabledFor(logging.INFO):
            logger.info("Task invoked by %s requesting workers %s on spill %d" \
                % (client_id, workers, spill_id))    
        if not isinstance(workers, ListType):
            raise ValueError("%s is not a list" % workers)
        # This does a start-to-end comparison of the lists.
        worker_list = list(workers)
        while len(worker_list) != 0:
            worker = worker_list.pop(0)
            if isinstance(worker, ListType):
                # Add sub-list members to front of list.
                worker.extend(worker_list)
                worker_list = worker
            else:
                # Process spill.
                if logger.isEnabledFor(logging.INFO):
                    logger.info("Executing worker : %s" % worker)
                if not isinstance(worker, StringType):
                    raise ValueError("Worker name %s is not a string" % worker)
                if not self.__transforms.has_key(worker):
                    raise ValueError("No such worker: %s" % worker)
                spill = self.__transforms[worker].process(spill)
        return spill

tasks.register(MausGenericTransformTask) # pylint:disable=W0104, E1101

from celery.worker.control import Panel

@Panel.register
def configure_maus(panel, configuration):
    print "Configuring..."
    from multiprocessing import current_process 
    print "Process name: %s" % current_process().name 
    pool = panel.consumer.pool 
    print "Pool processes: %s " % pool.info["processes"]
    maustask = tasks["mauscelery.maustasks.MausGenericTransformTask"]
    maustask.configure(configuration)
    print "Configured"
    return {"ok": "configured"}

@Panel.register
def reset_pool(panel):
    print "Resetting pool..."
    from multiprocessing import current_process 
    print "Process name: %s" % current_process().name 
    pool = panel.consumer.pool 
    print "Pool.information: %s" % pool.info
    pids = pool.info["processes"]
    try:
        import os 
        import signal
        for pid in pids:
            print "About to SIGTERM kill %s" % pid
            os.kill(pid, signal.SIGTERM)
            print "Killed %s" % pid
    except Exception: 
        traceback.print_exc(file=sys.stdout)
        print "Error"
    print "Reset"
    return {"ok": "reset"}
