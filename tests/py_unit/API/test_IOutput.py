"""
This file defines the tests for IOutput
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
from API.IOutput import IOutput

class TestIOutput(unittest.TestCase):
    """
    @class TestIOutput
    Unit testing class for IOutput
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            m = IOutput()
            self.assertIsInstance(m, IOutput, 'Not an instance of IOutput')
            self.assertIsInstance(m, IModule, 'Not an instance of IModule')
        except:
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_save(self):
        """Test save"""
        m = IOutput()
        self.assertRaises(NotImplementedError, m.save, 'data')

if __name__ == '__main__':
    unittest.main()
