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

class oldStyleClass:
    def __init__(self):
        self.a = 12
    @const
    def shouldntChange(self, val):
        self.a = val
        return self.a
    def shouldChange(self, val):
        self.a = val
        return self.a
    
class newStyleClass(object):
    def __init__(self):
        self.a = 12
    @const
    def shouldntChange(self, val):
        self.a = val
        return self.a
    def shouldChange(self, val):
        self.a = val
        return self.a

class TestConstable(unittest.TestCase):
    """
    @class TestConstable
    Unit testing class for Constable
    """
    def test_ConstMethodAttrModError__init__(self):
        """Test Exception Constructor"""
        try:
            m=ConstMethodAttrModError()
            self.assertIsInstance(m, ConstMethodAttrModError, 'Not an '\
                                  'instance of ConstMethodAttrModError')
            self.assertIsInstance(m, Exception, 'Not an instance of '\
                                  'Exception')
        except:
            self.assertTrue(False, 'Exception thrown when constructing')

    def test_oldStyleClass_shouldntChange(self):
        """Test shouldntChange method of old style classes"""
        m=oldStyleClass()
        self.assertRaises(ConstMethodAttrModError, m.shouldntChange, 24)

    def test_oldStyleClass_shouldChange(self):
        """Test shouldChange method of old style classes"""
        m=oldStyleClass()
        self.assertEqual(m.shouldChange(24), 24, 'failed to change the '\
                         'internal val in shouldChange method of old '\
                         'style class')

    def test_newStyleClass_shouldntChange(self):
        """Test shouldntChange method of new style classes"""
        m=newStyleClass()
        self.assertRaises(ConstMethodAttrModError, m.shouldntChange, 24)

    def test_newStyleClass_shouldChange(self):
        """Test shouldChange method of new style classes"""
        m=newStyleClass()
        self.assertEqual(m.shouldChange(24), 24, 'failed to change the '\
                         'internal val in shouldChange method of new '\
                         'style class')


if __name__ == '__main__':
    unittest.main()
