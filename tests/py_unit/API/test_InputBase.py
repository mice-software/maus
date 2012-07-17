"""
This file defines the tests for InputBase
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
from API.ModuleBase import ModuleBase
from API.InputBase import InputBase
from API.IInput import IInput

class InputBaseDummy(InputBase):
    def __init__(self):
        super(InputBaseDummy, self).__init__()
    def _emitter(self):
        return "MyData"

class TestInputBase(unittest.TestCase):
    """
    @class TestInputBase
    Unit testing class for InputBase
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            m = InputBase()
            self.assertIsInstance(m, InputBase, 'Not an instance of InputBase')
            self.assertIsInstance(m, IInput, 'Not an instance of IInput')
            self.assertIsInstance(m, ModuleBase, 'Not an instance of ModuleBase')
        except:
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_emitter(self):
        """Test emitter"""
        m = InputBaseDummy()
        self.assertEqual(m.emitter(), 'MyData', 'emitter method not '\
                         'calling _emitter properly')

    def test__emitter(self):
        """Test _emitter"""
        m = InputBase()
        self.assertRaises(NotImplementedError, m._emitter)

if __name__ == '__main__':
    unittest.main()
