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

"""Tests for MapCppTOFOfflineDigits"""

# pylint: disable = C0103

import os
import md5
import unittest
from Configuration import Configuration
from InputCppDAQOfflineData import InputCppDAQOfflineData

class InputCppDAQOfflineDataTestCase(unittest.TestCase): #pylint:disable=R0904
    """Tests for InputCppDAQOfflineData"""
    @classmethod
    def setUpClass(self): # pylint: disable = C0103, C0202
        """Sets a mapper and configuration"""
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        self._datapath = '%s/src/input/InputCppDAQData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        self._datafile = '02873'
        self._c = Configuration()
        self.mapper = None

    def test_init(self):
        """Check birth with default configuration"""
        self.mapper = InputCppDAQOfflineData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth( self._c.getConfigJSON() ))
        # Check re-init without closing fails
        self.assertFalse(self.mapper.birth( self._c.getConfigJSON() ))
        self.assertTrue(self.mapper.death())
        return

    def test_single(self):
        """Test a single event"""
        self.mapper = InputCppDAQOfflineData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth(self. _c.getConfigJSON() ))
        # Get a single event and check it's the right size
        self.assertTrue(self.mapper.readNextEvent())
        data = self.mapper.getCurEvent()
        # Data shold be 80 (first event is start of burst)
        self.assertEqual(len(data), 96)
        self.assertTrue(self.mapper.death())
        return

    def test_multi(self):
        """Test reading the whole file"""
        self.mapper = InputCppDAQOfflineData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.mapper.birth( self._c.getConfigJSON() ))
        event_count = 0

        # We can try md5'ing the whole dataset
        digester = md5.new()

        for i in self.mapper.emitter():
            digester.update(i) #pylint: disable=E1101
            event_count = event_count + 1

        # We should now have processed 26 events
        self.assertEqual(event_count, 26)

        # Check the md5 sum matches the expected value
        # changed checksum to reflect the run_num addition
        self.assertEqual(digester.hexdigest(), \
                         '15abbb1d734e6d72b4270a399af2406a')

        self.assertTrue(self.mapper.death())

    @classmethod
    def tearDownClass(self): # pylint: disable = C0103, C0202
        """Check that we can death() MapCppTOFDigits"""
        pass

if __name__ == '__main__':
    unittest.main()
