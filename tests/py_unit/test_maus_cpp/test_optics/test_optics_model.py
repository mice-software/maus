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

# pylint: disable=C0103

"""
Test maus_cpp.optics_model
"""

import json
import unittest
import StringIO

import Configuration
import maus_cpp.globals
import maus_cpp.optics_model
from maus_cpp.optics_model import OpticsModel

class OpticsModelTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.optics_model"""
    def setUp(self):
        """import datacards"""
        self.test_config = ""
        if maus_cpp.globals.has_instance():
            self.test_config = maus_cpp.globals.get_configuration_cards()
        else:
            self.test_config = Configuration.Configuration().getConfigJSON()
            maus_cpp.globals.birth(self.test_config)

    def test_init_no_globals(self):
        """Test maus_cpp.optics_model.__init__() and deallocation"""
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        try:
            OpticsModel()
            self.assertTrue(False, msg="Should throw an exception if globals "+\
                                       "is not initialised")
        except RuntimeError:
            pass

    def test_init_all_okay(self):
        """Test maus_cpp.optics_model.__init__() and deallocation"""
        if not maus_cpp.globals.has_instance():
            maus_cpp.globals.birth(self.test_config)
        optics = OpticsModel()
        optics.__init__() # legal, should reinitialise

if __name__ == "__main__":
    unittest.main()


