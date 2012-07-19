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
Test maus_cpp.run_action_manager
"""

import StringIO
import unittest

import Configuration

import maus_cpp.globals
import maus_cpp.run_action_manager

class GlobalsTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.run_action_manager"""

    def setUp(self): # pylint: disable=C0103
        """Set up test"""
        config_options = StringIO.StringIO(unicode("""
simulation_geometry_filename = "Test.dat"
reconstruction_geometry_filename = "Test.dat"
        """))
        self.config = Configuration.Configuration().getConfigJSON(
                                                         config_options, False)

    def test_start_end_of_run(self):
        """Test maus_cpp.run_action_manager.start_of_run(...)"""
        maus_cpp.globals.birth(self.config)
        maus_cpp.run_action_manager.start_of_run(1)
        maus_cpp.run_action_manager.end_of_run()
        


