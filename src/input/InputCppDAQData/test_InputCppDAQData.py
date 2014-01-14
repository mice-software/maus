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
# import md5
import unittest
# import json
from Configuration import Configuration
from InputCppDAQData import InputCppDAQData

class InputCppDAQDataTestCase(unittest.TestCase): # pylint: disable = R0904
    """Tests for InputCppDAQData"""
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
    def test_init(self):
        """Check birth with default configuration"""
        self._mapper = InputCppDAQData()
        self.assertTrue(self._mapper.birth( self._c.getConfigJSON() ))
        self.assertTrue(self._mapper.death())
        return

    @classmethod
    def tearDownClass(self): # pylint: disable = C0103,C0202
        """Check that we can death() MapCppTOFDigits"""
        pass

if __name__ == '__main__':
    unittest.main()
