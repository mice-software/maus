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

from celery.task import task
from celery.task import Task

from mauscelery.state import MausTransform

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
