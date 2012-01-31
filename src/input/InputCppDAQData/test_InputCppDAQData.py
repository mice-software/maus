# This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

"""Tests for MapCppTOFDigits"""

# pylint: disable = C0103

import os
import md5
import unittest
import json
from Configuration import Configuration
from InputCppDAQData import InputCppDAQData

class InputCppDAQDataTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for InputCppDAQData"""
    @classmethod
    def setUpClass(self): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        self._datapath = '%s/src/input/InputCppDAQData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        self._datafile = '02873'
        self._c = Configuration()

    def test_init(self):
        """Check birth with default configuration"""
        self.mapper = InputCppDAQData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth( self._c.getConfigJSON() ))
        # Check re-init without closing fails
        self.assertFalse(self.mapper.birth( self._c.getConfigJSON() ))
        self.assertTrue(self.mapper.death())
        return

    def test_single(self):
        """Test a single event"""
        self.mapper = InputCppDAQData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth(self. _c.getConfigJSON() ))
        # Get a single event and check it's the right size
        self.assertTrue(self.mapper.readNextEvent())
        data = self.mapper.getCurEvent()
        # Data shold be 65 (an empty spill, first event is start of burst)        print data, len(data)
        # len changed due to run_num addition
        self.assertEqual(len(data), 80)
        self.assertTrue(self.mapper.death())
        return

    def test_multi(self):
        """Test reading the whole file"""
        self.mapper = InputCppDAQData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth( self._c.getConfigJSON() ))
        event_count = 0

        # We can try md5'ing the whole dataset
        digester = md5.new()

        for i in self.mapper.emitter():
            digester.update(i)
            event_count = event_count + 1

        # We should now have processed 26 events
        self.assertEqual(event_count, 26)

        # Check the md5 sum matches the expected value
        # changed checksum to reflect the run_num addition
        self.assertEqual(digester.hexdigest(), \
                         '426cc54172fc1091185f4eaacdd8b85a')

        self.assertTrue(self.mapper.death())

    @classmethod
    def tearDownClass(self): # pylint: disable = C0103
        """Check that we can death() MapCppTOFDigits"""
        pass

if __name__ == '__main__':
    unittest.main()
