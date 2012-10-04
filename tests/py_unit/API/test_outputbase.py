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
    '''Test outputter'''
    def __init__(self):
        '''constructor'''
        super(OutputBaseDummy, self).__init__()
    def _birth(self, config):
        '''_birth'''
        pass
    def _death(self):
        '''_death'''
        pass
    def _save(self, data):
        return True

class TestOutputBase(unittest.TestCase):#pylint: disable=R0904
    """
    @class TestOutputBase
    Unit testing class for OutputBase
    """
    def test___init__(self):
        """Test Constructor"""
        try:
            mmm = OutputBase()
            self.assertIsInstance(mmm,
                                  OutputBase,
                                  'Not an instance of OutputBase')
            self.assertIsInstance(mmm,
                                  IOutput,
                                  'Not an instance of IOutput')
            self.assertIsInstance(mmm,
                                  ModuleBase,
                                  'Not an instance of ModuleBase')
        except: #pylint: disable=W0702
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_save(self):
        """Test save"""
        mmm = OutputBaseDummy()
        self.assertTrue(mmm.save('MyData'), 'save method not '\
                        'calling _save properly')

    def test__save(self):
        """Test _save"""
        mmm = OutputBase()
        self.assertRaises(NotImplementedError,
                          mmm._save, 'data')#pylint: disable=W0212

if __name__ == '__main__':
    unittest.main()
