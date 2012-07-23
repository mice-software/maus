"""
This file defines the tests for ModuleBase
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
from API.IModule import IModule

class ModuleBaseDummy(ModuleBase):
    '''Test Module'''
    def __init__(self):
        '''constructor'''
        super(ModuleBaseDummy, self).__init__()
    def _birth(self, config):
        '''_brith'''
        return '_birth ran'
    def _death(self):
        '''_death'''
        return '_death ran'

class TestModuleBase(unittest.TestCase):#pylint: disable=R0904
    """
    @class TestModuleBase
    Unit testing class for ModuleBase
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            mmm = ModuleBase()
            self.assertIsInstance(mmm,
                                  ModuleBase,
                                  'Not an instance of ModuleBase')
            self.assertIsInstance(mmm,
                                  IModule,
                                  'Not an instance of IModule')
        except: #pylint: disable=W0702
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_birth(self):
        """Test birth"""
        mmm = ModuleBaseDummy()
        self.assertEqual(mmm.birth('MyConfig'),
                         '_birth ran',
                         'process method not calling _birth properly')

    def test__birth(self):
        """Test _birth"""
        mmm = ModuleBase()
        self.assertRaises(NotImplementedError,
                          mmm._birth, 'MyConfig')#pylint: disable=W0212

    def test_death(self):
        """Test death"""
        mmm = ModuleBaseDummy()
        self.assertEqual(mmm.death(), '_death ran', 'process method not '\
                         'calling _death properly')

    def test__death(self):
        """Test _death"""
        mmm = ModuleBase()
        self.assertRaises(NotImplementedError, mmm._death)#pylint: disable=W0212

if __name__ == '__main__':
    unittest.main()
