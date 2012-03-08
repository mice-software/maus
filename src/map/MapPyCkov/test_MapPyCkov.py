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
import json
import MapPyCkov

class TestMapPyCkov(unittest.TestCase): # pylint: disable = R0904
    """set of tests for MapPyCkov"""
 
    def test_process_bad_event(self):
      test_doc = json.dumps({"daq_data":{}})
      mpc = MapPyCkov.MapPyCkov()
      mpc.birth(json.dumps({}))
      self.assertEqual(test_doc, mpc.process(test_doc))

if __name__ == "__main__":
    unittest.main()

