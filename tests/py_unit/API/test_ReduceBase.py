"""
This file defines the tests for ReduceBase
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
from API.ReduceBase import ReduceBase
from API.IReduce import IReduce

class ReduceBaseDummy(ReduceBase):
    def __init__(self):
        super(ReduceBaseDummy, self).__init__()
    def _process(self, data):
        return data

class TestReduceBase(unittest.TestCase):
    """
    @class TestReduceBase
    Unit testing class for ReduceBase
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            m=ReduceBase()
            self.assertIsInstance(m, ReduceBase, 'Not an instance of ReduceBase')
            self.assertIsInstance(m, IReduce, 'Not an instance of IReduce')
            self.assertIsInstance(m, ModuleBase, 'Not an instance of ModuleBase')
        except:
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_process(self):
        """Test process"""
        m=ReduceBaseDummy()
        self.assertEqual(m.process('MyData'), 'MyData', 'process method not '\
                         'calling _process properly')

    def test__process(self):
        """Test _process"""
        m=ReduceBase()
        self.assertRaises(NotImplementedError, m._process, 'MyData')

if __name__ == '__main__':
    unittest.main()
