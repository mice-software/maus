#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

"""Tests for PyWrapMapBase and API"""

import sys 
import unittest
import json

from _MapCppExampleMAUSDataInput import MapCppExampleMAUSDataInput

import Configuration

class TestMapCppExample(unittest.TestCase): #pylint: disable=R0904
    """Tests for PyWrapMapBase and API"""
    def setUp(self): # pylint: disable=C0103
        """Setup the tests"""
        self.config = Configuration.Configuration().getConfigJSON()

    def test_new_init_dealloc(self):
        """MapCppExample - Test we can init"""
        map_example = MapCppExampleMAUSDataInput() # pylint: disable=C0103
        ref_tmp = [] # note this has to be mutable object
        self.assertEqual(sys.getrefcount(map_example), sys.getrefcount(ref_tmp))

    def test_birth(self):
        """MapCppExample - test_birth"""
        map_example = MapCppExampleMAUSDataInput()
        self.assertRaises(TypeError, map_example.birth)
        self.assertRaises(TypeError, map_example.birth, (1,))
        self.assertRaises(TypeError, map_example.birth, ("", ""))
        map_example.birth(json.dumps(self.config))
        self.assertRaises(ValueError, map_example.birth, "")

    def test_death(self): #pylint: disable=R0201
        """MapCppExample - test death"""
        map_example = MapCppExampleMAUSDataInput()
        map_example.death()

    def test_process(self):
        """MapCppExample - test process and conversions"""
        data_1 = \
        '{"daq_event_type":"","errors":{},"maus_event_type":"Spill",'+\
        '"run_number":99,"spill_number":666}'
        map_example = MapCppExampleMAUSDataInput()
        map_example.birth(json.dumps(self.config))
        data_2 = map_example.process(data_1)
        self.assertEqual(data_2.GetSpill().GetSpillNumber(),
                         667)
        data_3 = map_example.process(data_2)
        self.assertEqual(data_3.GetSpill().GetSpillNumber(),
                         data_2.GetSpill().GetSpillNumber()+1)

    def test_can_convert(self):
        """MapCppExample - test can_convert flag"""
        self.assertTrue(MapCppExampleMAUSDataInput().can_convert)

    def test_import(self):
        """MapCppExample - test import from MAUS module"""
        from MAUS import MapCppExampleMAUSDataInput as tmp
        self.assertEqual(tmp, MapCppExampleMAUSDataInput)

if __name__ == "__main__":
    unittest.main()
