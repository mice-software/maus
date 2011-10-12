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

"""
Tests for input cpp data
"""

#pylint: disable = C0103

import os
import md5
import unittest
import json

from Configuration import Configuration
from InputCppDAQData import InputCppDAQData

class InputCppDAQDataTestCase(unittest.TestCase): #pylint: disable = R0904
    """
    We test input cpp data by checking unpacked files using an md5sum. Note that
    this is rather quick and dirty...
    """
    @classmethod
    def setUpClass(cls):
        """
        Setup the data file and configuration
        """
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        cls._datapath = '%s/src/input/InputCppDAQData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        cls._datafile = '02873'
        cls._c = Configuration()

    def setUp(self):
        """
        Setup the InputCppDAQData mapper
        """
        self.mapper = InputCppDAQData(self._datapath, self._datafile)

    def test_init(self):
        """
        Check that we can call birth on the data file
        """
        self.assertTrue(self.mapper.birth( self._c.getConfigJSON() ))
        # Check re-init without closing fails
        self.assertFalse(self.mapper.birth( self._c.getConfigJSON() ))
        self.assertTrue(self.mapper.death())
        return

    def test_single(self):
        """
        Test a single spill
        """
        self.assertTrue(self.mapper.birth(self. _c.getConfigJSON() ))
        # Get a single event and check it's the right size
        data = self.mapper.getNextEvent()
        # Data shold be 66 (an empty spill, first event is start of burst)
        self.assertEqual(len(data), 67)
        self.assertTrue(self.mapper.death())
        return

    def test_multi(self):
        """
        Test reading the whole file
        """
        self.assertTrue(self.mapper.birth( self._c.getConfigJSON() ))
        spill_count = 0
        event_count = 0

        # We can try md5'ing the whole dataset
        digester = md5.new()

        for inp in self.mapper.emitter():
            digester.update(inp)
            json_in = json.loads(inp)
            spill_count = spill_count + 1
            event_count += len(json_in["daq_data"])

        # We should now have processed 8 spills
        self.assertEqual(spill_count, 8)
        self.assertEqual(event_count, 26)

        # Check the md5 sum matches the expected value
        self.assertEqual(digester.hexdigest(),
                         'f4f50073b67d488f8b678a969b8564f6')

        self.assertTrue(self.mapper.death())

    @classmethod
    def tearDownClass(cls):
        pass

if __name__ == '__main__':
    unittest.main()
