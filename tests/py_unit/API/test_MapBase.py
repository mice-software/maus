"""
This file defines the tests for MapBase
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
from API.MapBase import MapBase
from API.IMap import IMap
from API.Constable import ConstMethodAttrModError

class MapBaseDummy(MapBase):
    def __init__(self):
        super(MapBaseDummy, self).__init__()
    def _process(self, data):
        return data
class MapBaseDummyConstTest(MapBase):
    def __init__(self):
        super(MapBaseDummyConstTest, self).__init__()
        self.my_var = 12
    def _process(self, data):
        self.my_var = 13
        return data
    
class TestMapBase(unittest.TestCase):
    """
    @class TestMapBase
    Unit testing class for MapBase
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            m=MapBase()
            self.assertIsInstance(m, MapBase, 'Not an instance of MapBase')
            self.assertIsInstance(m, IMap, 'Not an instance of IMap')
            self.assertIsInstance(m, ModuleBase, 'Not an instance of ModuleBase')
        except:
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_process(self):
        """Test process"""
        m=MapBaseDummy()
        self.assertEqual(m.process('MyData'), 'MyData', 'process method not '\
                         'calling _process properly')
        m=MapBaseDummyConstTest()
        self.assertRaises(ConstMethodAttrModError, m.process, 'MyData')

    def test__process(self):
        """Test _process"""
        m=MapBase()
        self.assertRaises(NotImplementedError, m._process, 'MyData')

if __name__ == '__main__':
    unittest.main()
