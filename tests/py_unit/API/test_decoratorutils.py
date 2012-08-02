"""
This file defines the tests for DecoratorUtils
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
import inspect
from API.DecoratorUtils import smart_decorator

# regular decorator function
def test_decorator(func):
    '''Standard decorator'''
    def wrapper(self, *args, **kw):
        '''wrapper'''
        return func(self, *args, **kw)
    return wrapper

# smart decorator function
@smart_decorator
def test_smartdecorator(func):
    '''Smart decorated decortor'''
    def wrapper(self, *args, **kw):
        '''wrapper'''
        return func(self, *args, **kw)
    return wrapper

# these are the test functions to be decorated
# ############################################

# the undecorated control function
def testingfunc1(a, b, c, d=22, *testArgs, **testKwds):#pylint: disable=C0103,W0613,C0301
    '''Control un-decorated function'''
    pass
# regular 'dumb' decorator
@test_decorator
def testingfunc2(a, b, c, d=22, *testArgs, **testKwds):#pylint: disable=C0103,W0613,C0301
    '''Function decorated with regular dumb decorator'''
    pass
# smart decorator
@test_smartdecorator
def testingfunc3(a, b, c, d=22, *testArgs, **testKwds):#pylint: disable=C0103,W0613,C0301
    '''Function decorated with smart decorator'''
    pass

class TestDecoratorUtils(unittest.TestCase):#pylint: disable=R0904
    """
    @class TestDecoratorUtils
    Unit testing class for DecoratorUtils
    """
    def test_smart_decorator(self):
        """Test that smart decorators correctly duplicate the ArgSpec and name
        of the underlying function"""
        argspec = inspect.ArgSpec( args = ['a', 'b', 'c', 'd'],
                                   varargs = 'testArgs',
                                   keywords = 'testKwds',
                                   defaults = (22,) )

        self.assertEqual(inspect.getargspec(testingfunc1), argspec)
        self.assertNotEqual(inspect.getargspec(testingfunc2), argspec)
        self.assertEqual(inspect.getargspec(testingfunc3), argspec)

        self.assertEqual(testingfunc1.__name__, 'testingfunc1')
        self.assertNotEqual(testingfunc2.__name__, 'testingfunc2')
        self.assertEqual(testingfunc3.__name__, 'testingfunc3')

if __name__ == '__main__':
    unittest.main()
