"""
MAUS Celery tasks. This forms the main API offered to Celery clients
to invoke a transform on a spill.
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
import json

from celery.task import task
from celery.task import Task

import maus_cpp.converter

from framework.workers import WorkerProcessException
from mauscelery.state import MausConfiguration
from mauscelery.state import MausTransform

# NOTE that the time_limit is hard coded! Decorators require hard codedness;
# the time_limit parameter appears to be available as a parameter to celery
# (see https://github.com/celery/celery/issues/802) but appears to require
# celery > 3.0 or so. I believe decorators are resolved when they are first
# encountered by the parser (but others may know more) - Rogers
@task(name="mauscelery.maustasks.MausGenericTransformTask", time_limit=100)
def execute_transform(spill, client_id = "Unknown"):
    """
    MAUS Celery transform task used by sub-processes to execute jobs
    from clients. Proxies of this task are invoked by clients.This
    applies the current transform to the spill and returns the new
    spill.  
    @param spill JSON document string holding spill.
    @param client_id ID of client who submitted job.
    @return JSON document string holding new spill.
    @throws Exception if there is a problem when process is called.
    """
    logger = Task.get_logger()  
    if logger.isEnabledFor(logging.INFO):
        logger.info("Task invoked by %s" % client_id)
    try:
        spill_json = json.loads(spill)
        if "maus_event_type" in spill_json.keys() and \
           spill_json["maus_event_type"] != "Spill":
            return spill
        else:
            return maus_cpp.converter.string_repr(MausTransform.process(spill))
    except Exception as exc: # pylint:disable = W0703
        # Filter exceptions so no unPicklable exception causes
        # problems.
        status = {}
        status["error"] = str(exc.__class__)
        status["message"] = str(exc)
        raise WorkerProcessException(MausConfiguration.transform,
            status)
