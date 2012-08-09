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
    '''Test class'''
    def __init__(self):
        '''constructor'''
        super(MapBaseDummy, self).__init__()
    def _birth(self, config):
        '''_birth'''
        pass
    def _death(self):
        '''_death'''
        pass
    def _process(self, data):
        '''_process'''
        return data
class MapBaseDummyConstTest(MapBase):
    '''Test class violating constness'''
    def __init__(self):
        '''constructor'''
        super(MapBaseDummyConstTest, self).__init__()
        self.my_var = 12
    def _birth(self, config):
        '''_birth'''
        pass
    def _death(self):
        '''_death'''
        pass
    def _process(self, data):
        '''_process'''
        self.my_var = 13
        return data
    
class TestMapBase(unittest.TestCase):#pylint: disable=R0904
    """
    @class TestMapBase
    Unit testing class for MapBase
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            mmm = MapBase()
            self.assertIsInstance(mmm,
                                  MapBase,
                                  'Not an instance of MapBase')
            self.assertIsInstance(mmm,
                                  IMap,
                                  'Not an instance of IMap')
            self.assertIsInstance(mmm,
                                  ModuleBase,
                                  'Not an instance of ModuleBase')
        except: #pylint: disable=W0702
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_process(self):
        """Test process"""
        mmm = MapBaseDummy()
        self.assertEqual(mmm.process('MyData'), 'MyData', 'process method not '\
                         'calling _process properly')
        mmm = MapBaseDummyConstTest()
        self.assertRaises(ConstMethodAttrModError, mmm.process, 'MyData')

    def test__process(self):
        """Test _process"""
        mmm = MapBase()
        self.assertRaises(NotImplementedError,
                          mmm._process, 'MyData')#pylint: disable=W0212

if __name__ == '__main__':
    unittest.main()
