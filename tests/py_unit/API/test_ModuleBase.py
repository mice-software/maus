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
    def __init__(self):
        super(ModuleBaseDummy, self).__init__()
    def _birth(self, config):
        return '_birth ran'
    def _death(self):
        return '_death ran'

class TestModuleBase(unittest.TestCase):
    """
    @class TestModuleBase
    Unit testing class for ModuleBase
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            m=ModuleBase()
            self.assertIsInstance(m, ModuleBase, 'Not an instance of ModuleBase')
            self.assertIsInstance(m, IModule, 'Not an instance of IModule')
        except:
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_birth(self):
        """Test birth"""
        m=ModuleBaseDummy()
        self.assertEqual(m.birth('MyConfig'), '_birth ran', 'process method not '\
                         'calling _birth properly')

    def test__birth(self):
        """Test _birth"""
        m=ModuleBase()
        self.assertRaises(NotImplementedError, m._birth, 'MyConfig')

    def test_death(self):
        """Test death"""
        m=ModuleBaseDummy()
        self.assertEqual(m.death(), '_death ran', 'process method not '\
                         'calling _death properly')

    def test__death(self):
        """Test _death"""
        m=ModuleBase()
        self.assertRaises(NotImplementedError, m._death)

if __name__ == '__main__':
    unittest.main()
