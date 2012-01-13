"""
A custom loader for Celery which ensures that all tasks available
to workers are initialised with MAUS configuration.
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

import sys
import traceback

from celery import registry
from celery.loaders.default import Loader

from Configuration import Configuration

class TaskBirthException(Exception):
    """ Exception raised if a MAUS task birth call failed. """

    def __init__(self, task, cause = None):
        """
        Constructor. Overrides Exception.__init__.
        @param self Object reference.
        @param task Name of task that failed to be birthed.
        @param cause Optional causal exception.
        """
        Exception.__init__(self)
        self.__cause = cause
        self.__task = task

    def __str__(self):
        """
        Return string representation. Overrides Exception.__str__.
        @param self Object reference.
        @return string.
        """
        if (self.__cause == None):
            return "%s.birth returned False" % self.__task
        else:
            return "%s.birth failed due to: %s" % (self.__task, self.__cause)
           
    @property
    def cause(self):
        """
        Getter for exception cause.
        @param self Object reference.
        @return cause or None.
        """
        return self.__cause

    @property
    def task(self):
        """ 
        Getter for task name.
        @param self Object reference.
        @return task name.
        """
        return self.__task

class CeleryLoader(Loader):
    """
    Class mausloader.CeleryLoader
    A custom loader for Celery which ensures that all tasks available
    to workers are initialised with MAUS configuration.
    """

    def on_worker_init(self):
        """
        Method called when the worker starts.
        Invokes super-class method, loads default MAUS configuration,
        birth is then invoked on each task that supports this method
        and the MAUS JSON configuration passed to them.
        @param self Object reference.
        @throws TaskBirthException if there is a problem in invoking
        birth.
        """
        # Load modules specified in CELERY_IMPORTS list.
        Loader.on_worker_init(self)

        # Get the default MAUS configuration.
        config = Configuration()
        # Load the MAUS configuration.
        config_doc = config.getConfigJSON()
        # Initialise and birth MAUS tasks.
        for celery_task_name in registry.tasks:
            celery_task = registry.tasks[celery_task_name]
            was_birthed = False
            if hasattr(celery_task, "birth"):
                try: 
                    was_birthed = celery_task.birth(config_doc)
                except Exception, birth_exception: 
                    # Print then raise new exception.
                    exc_info = sys.exc_info()
                    traceback.print_exception(exc_info[0],
                                              exc_info[1],
                                              exc_info[2],
                                              None, 
                                              sys.__stderr__)
                    raise TaskBirthException(celery_task_name, 
                                             birth_exception)
                if not was_birthed:
                    raise TaskBirthException(celery_task_name)
