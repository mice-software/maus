"""
MAUS Celery generic node inspection and control.
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

from multiprocessing import current_process 
from celery.worker.control import Panel

@Panel.register
def get_process_pool(panel):
    """
    Get information on the current process pool.
    @param panel Celery panel object.
    @return status document of form {master_pid: PID, master_name:
    PROCESS_NAME, pool_pids: [PID, PID, PID]}.  
    """
    doc = {}
    process_status = current_process()
    doc["master_pid"] = process_status.pid
    doc["master_name"] = process_status.name
    pool = panel.consumer.pool 
    doc["pool_pids"] = pool.info["processes"]
    logger = logging.getLogger(__name__)
    if logger.isEnabledFor(logging.INFO):
        logger.info("Process pool: %s" % doc)
    return doc

@Panel.register
def restart_pool(panel):
    """
    Restart the worker pool by killing the pool processes via a
    pool.terminate_job invocation, which forces Celery to spawn
    new pool processes.  
    @param panel Celery panel object.
    @return status document of form {status:"ok"} if all went well
    or {status:"error", error:EXCEPTION_CLASS,
    message:EXCEPTION_MESSAGE} if an exception occurred.
    """
    logger = logging.getLogger(__name__)
    if logger.isEnabledFor(logging.INFO):
        logger.info("Restarting pool")
    pool = panel.consumer.pool 
    pids = pool.info["processes"]
    doc = {}
    try:
        for pid in pids:
            if logger.isEnabledFor(logging.INFO):
                logger.info("About to terminate %s" % pid)
            pool.terminate_job(pid)
            if logger.isEnabledFor(logging.INFO):
                logger.info("Terminated %s" % pid)
            doc["status"] = "ok"
    except Exception as ex: # pylint:disable=W0703
        if logger.isEnabledFor(logging.ERROR):
            logger.exception(ex)
        doc["status"] = "error"
        doc["error"] = str(ex.__class__)
        doc["message"] = ex.message
    if logger.isEnabledFor(logging.INFO):
        logger.info("Status: %s" % doc)
    return doc
