"""
This file defines the tests for OutputBase
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
from API.OutputBase import OutputBase
from API.IOutput import IOutput

class OutputBaseDummy(OutputBase):
    def __init__(self):
        super(OutputBaseDummy, self).__init__()
    def _save(self, data):
        return True

class TestOutputBase(unittest.TestCase):
    """
    @class TestOutputBase
    Unit testing class for OutputBase
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            m=OutputBase()
            self.assertIsInstance(m, OutputBase, 'Not an instance of OutputBase')
            self.assertIsInstance(m, IOutput, 'Not an instance of IOutput')
            self.assertIsInstance(m, ModuleBase, 'Not an instance of ModuleBase')
        except:
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_save(self):
        """Test save"""
        m=OutputBaseDummy()
        self.assertTrue(m.save('MyData'), 'save method not '\
                        'calling _save properly')

    def test__save(self):
        """Test _save"""
        m=OutputBase()
        self.assertRaises(NotImplementedError, m._save, 'data')

if __name__ == '__main__':
    unittest.main()
