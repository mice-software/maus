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

import unittest

from mauscelery.state import MausConfiguration
from mauscelery.mausworker import get_maus_configuration

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

if __name__ == '__main__':
    unittest.main()
