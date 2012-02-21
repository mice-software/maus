"""
Test class for mauscelery.mausworker module.
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
import unittest

from Configuration import Configuration
import mauscelery.mausworker
from mauscelery.state import MausConfiguration
from mauscelery.mausworker import worker_init_callback
from mauscelery.mausworker import get_maus_configuration

class WorkerInitCallbackTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.mausworker.worker_init_callback 
    method.
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Reset MausConfiguration and set logging.
        @param self Object reference.
        """
        MausConfiguration.version = None
        # Configure lowest logging level so all logging statements
        # are executed.
        logger = logging.getLogger(mauscelery.mausworker.__name__)
        logger.setLevel(logging.DEBUG)

    def test_callback(self):
        """ 
        Invoke worker_process_callback.
        @param self Object reference.
        """
        worker_init_callback()
        configuration  = Configuration()
        config_doc = configuration.getConfigJSON()
        config_dictionary = json.loads(config_doc)
        self.assertEquals(config_dictionary["maus_version"],
            MausConfiguration.version,
            "Unexpected version")

class GetMausConfigPanelTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.mausworker.get_maus_configuration method. 
    """

    def test_panel_get_config(self):
        """ 
        Test the get_maus_configuration panel operation.
        @param self Object reference.
        """
        MausConfiguration.transform = "MapPyTestMap"
        MausConfiguration.configuration = "{test}"
        MausConfiguration.version = "MAUS release version: 123"
        config = get_maus_configuration(None)
        self.assertTrue(config.has_key("config_id"), 
            "Missing config_id key")
        self.assertEquals(MausConfiguration.config_id, 
            config["config_id"], "Unexpected config_id")
        self.assertTrue(config.has_key("configuration"), 
            "Missing configuration key")
        self.assertEquals(MausConfiguration.configuration, 
            config["configuration"], "Unexpected configuration")
        self.assertTrue(config.has_key("transform"),
            "Missing transform")
        self.assertEquals(MausConfiguration.transform,
            config["transform"], "Unexpected transform")
        self.assertEquals(MausConfiguration.version,
            config["version"], "Unexpected version")

if __name__ == '__main__':
    unittest.main()
