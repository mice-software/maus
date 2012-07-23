"""
This file defines the tests for Constable
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
from API.Constable import const, ConstMethodAttrModError

class Oldstyleclass:
    """Old style class"""
    def __init__(self):
        '''constructor'''
        self.aaa = 12
    @const
    def shouldntchange(self, val):
        '''const method'''
        self.aaa = val
        return self.aaa
    def shouldchange(self, val):
        '''non-const method'''
        self.aaa = val
        return self.aaa
    
class Newstyleclass(object):
    """New style class"""
    def __init__(self):
        '''constructor'''
        self.aaa = 12
    @const
    def shouldntchange(self, val):
        '''const method'''
        self.aaa = val
        return self.aaa
    def shouldchange(self, val):
        '''non-const method'''
        self.aaa = val
        return self.aaa

class TestConstable(unittest.TestCase):#pylint: disable=R0904
    """
    @class TestConstable
    Unit testing class for Constable
    """
    def test_ConstMethodAttrModError__init__(self):#pylint: disable=C0103
        """Test Exception Constructor"""
        try:
            mmm = ConstMethodAttrModError()
            self.assertIsInstance(mmm, ConstMethodAttrModError, 'Not an '\
                                  'instance of ConstMethodAttrModError')
            self.assertIsInstance(mmm, Exception, 'Not an instance of '\
                                  'Exception')
        except: #pylint: disable=W0702
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_oldstyleclass_shouldntchange(self):#pylint: disable=C0103
        """Test shouldntchange method of old style classes"""
        mmm = Oldstyleclass()
        self.assertRaises(ConstMethodAttrModError, mmm.shouldntchange, 24)

    def test_oldstyleclass_shouldchange(self):
        """Test shouldchange method of old style classes"""
        mmm = Oldstyleclass()
        self.assertEqual(mmm.shouldchange(24), 24, 'failed to change the '\
                         'internal val in shouldchange method of old '\
                         'style class')

    def test_newstyleclass_shouldntchange(self):#pylint: disable=C0103
        """Test shouldntchange method of new style classes"""
        mmm = Newstyleclass()
        self.assertRaises(ConstMethodAttrModError, mmm.shouldntchange, 24)

    def test_newstyleclass_shouldchange(self):
        """Test shouldchange method of new style classes"""
        mmm = Newstyleclass()
        self.assertEqual(mmm.shouldchange(24), 24, 'failed to change the '\
                         'internal val in shouldchange method of new '\
                         'style class')


if __name__ == '__main__':
    unittest.main()
