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

from celery import registry
from celery.loaders.default import Loader

from Configuration import Configuration

class CeleryLoader(Loader):
    """
    Class mausloader.CeleryLoader
    A custom loader for Celery which ensures that all tasks available
    to workers are initialised with MAUS configuration.
    """

    def __init__(self, app, **kwargs):
        """
        Prints a message then invokes superclass constructor.
        @param self Object reference.
        """
        print "---->MAUSCeleryLoader.__init"
        Loader.__init__(self, app, **kwargs)
        print "<----MAUSCeleryLoader.__init"

    def on_worker_init(self):
        """
        Invokes super-class method, loads default MAUS configuration,
        loads additional configuration if a Celery configuration file
        MAUS_CONFIG_FILE property is provided. birth is then invoked
        on each task that supports this method and the MAUS JSON 
        configuration passed to them.
        @param self Object reference.
        """
        print "---->MAUSCeleryLoader.__on_worker_init"

        # Load modules specified in CELERY_IMPORTS list.
        Loader.on_worker_init(self)

        # Get the default MAUS configuration.
        config = Configuration()
        # Get a MAUS configuration file, if specified in the Celery
        # configuration.
        if self.conf.has_key("MAUS_CONFIG_FILE"):
            config_file_name = self.conf["MAUS_CONFIG_FILE"]
            try:
                config_file = open(config_file_name, "r")
            except IOError:
                raise ValueError, \
                    ("MAUS_CONFIG_FILE %s does not exist" % config_file_name), \
                     sys.exc_info()[2]
        else:
            config_file = None
        # Load the MAUS configuration.
        config_doc = config.getConfigJSON(config_file)
        # Initialise the MAUS tasks.
        print "MAUSCeleryLoader Initialising MAUS tasks"
        for celery_task_name in registry.tasks:
            celery_task = registry.tasks[celery_task_name]
            if hasattr(celery_task, "birth"):
                print "MAUSCeleryLoader MAUS task: %s. Invoking birth()" \
                    % celery_task_name
                celery_task.birth(config_doc)
        print "MAUSCeleryLoader MAUS tasks initialised"
        print "<----MAUSCeleryLoader.__on_worker_init"
