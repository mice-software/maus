"""
This file defines the tests for IInput
"""
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
from API.IModule import IModule
from API.IInput import IInput

class TestIInput(unittest.TestCase):#pylint: disable=R0904
    """
    @class TestIInput
    Unit testing class for IInput
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            mmm = IInput()
            self.assertIsInstance(mmm, IInput, 'Not an instance of IInput')
            self.assertIsInstance(mmm, IModule, 'Not an instance of IModule')
        except: #pylint: disable=W0702
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_emitter(self):
        """Test emitter"""
        mmm = IInput()
        self.assertRaises(NotImplementedError, mmm.emitter)

if __name__ == '__main__':
    unittest.main()
