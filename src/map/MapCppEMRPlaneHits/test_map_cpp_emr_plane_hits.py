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

"""Tests for MapCppEMRPlaneHits"""

import json
import unittest
from MAUS import MapCppEMRPlaneHits

class TestMapCppEMRPlaneHits(unittest.TestCase): #pylint: disable=R0904
    """Tests for MapCppEMRPlaneHits"""
    def setUp(self): #pylint: disable=C0103
        """Set up a minimal spill"""
        self.minimal_spill = {"spill_number":1,
                              "run_number":-1,
                              "daq_event_type":"physics_event",
                              "errors":{"an_error":"message"},
                              "maus_event_type":"Spill"}
        self.mapper = MapCppEMRPlaneHits()

    def tearDown(self): #pylint: disable=C0103
        """Does nothing"""
        pass

    def test_json_convert(self):
        """Test the json conversion works okay"""
        test_output = self.mapper.process(json.dumps(self.minimal_spill))
        self.assertEqual(json.loads(test_output), self.minimal_spill)

if __name__ == "__main__":
    unittest.main()

