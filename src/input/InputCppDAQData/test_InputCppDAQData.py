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

"""Tests for InputCppDAQData"""

# pylint: disable = C0103

import os
import md5
import unittest
from Configuration import Configuration
from InputCppDAQData import InputCppDAQData

class InputCppDAQDataTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for InputCppDAQData"""

    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Set up location for data file"""
        if not os.environ.get("MAUS_ROOT_DIR"):
            raise Exception('InitializeFail', 'MAUS_ROOT_DIR unset!')
        # Set our data path & filename
        # It would be nicer to test with a smaller data file!
        cls._datapath = '%s/src/input/InputCppDAQData' % \
                            os.environ.get("MAUS_ROOT_DIR")
        cls._datafile = '02873'
        cls._c = Configuration()
        cls.input = None

    def test_init(self):
        """
        Set up location for data file; check we can initialise, birth and death
        properly
        """
        self.input = InputCppDAQData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.input.birth( self._c.getConfigJSON() ))
        # Check re-init without closing fails
        self.assertFalse(self.input.birth( self._c.getConfigJSON() ))
        self.assertTrue(self.input.death())
        return

    # Test a single event
    def test_single(self):
        """Test for single input datum"""
        self.input = InputCppDAQData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.input.birth(self. _c.getConfigJSON() ))
        # Get a single event and check it's the right size
        self.assertTrue(self.input.readNextEvent())
        data = self.input.getCurEvent()
        # Data shold be 66 (an empty spill, first event is start of burst)
        self.assertEqual(len(data), 65)
        self.assertTrue(self.input.death())
        return

    # Test reading the whole file
    def test_multi(self):
        """Test for multiple input data"""
        self.input = InputCppDAQData(self._datapath, \
                                       self._datafile)
        self.assertTrue(self.input.birth( self._c.getConfigJSON() ))
        event_count = 0
        # We can try md5'ing the whole dataset
        digester = md5.new()
        for i in self.input.emitter():
            digester.update(i)
            event_count = event_count + 1
        # We should now have processed 26 events
        self.assertEqual(event_count, 26)
        # Check the md5 sum matches the expected value
        self.assertEqual(digester.hexdigest(),
                         '6d925662f1844ff0198d7e810bbe9f0c')
        self.assertTrue(self.input.death())

if __name__ == '__main__':
    unittest.main()
