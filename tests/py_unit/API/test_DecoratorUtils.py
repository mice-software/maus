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
def testDecorator(func):
    def wrapper(self, *args, **kw):
        return func(self, *args, **kw)
    return wrapper

# smart decorator function
@smart_decorator
def testSmartDecorator(func):
    def wrapper(self, *args, **kw):
        return func(self, *args, **kw)
    return wrapper

# these are the test functions to be decorated
# ############################################

# the undecorated control function
def testingFunc1(a, b, c, d=22, *testArgs, **testKwds):
    pass
# regular 'dumb' decorator
@testDecorator
def testingFunc2(a, b, c, d=22, *testArgs, **testKwds):
    pass
# smart decorator
@testSmartDecorator
def testingFunc3(a, b, c, d=22, *testArgs, **testKwds):
    pass

class TestDecoratorUtils(unittest.TestCase):
    """
    @class TestDecoratorUtils
    Unit testing class for DecoratorUtils
    """
    def test_smart_decorator(self):
        argspec = inspect.ArgSpec( args = ['a', 'b', 'c', 'd'],
                                   varargs = 'testArgs',
                                   keywords = 'testKwds',
                                   defaults = (22,) )

        self.assertEqual(inspect.getargspec(testingFunc1), argspec)
        self.assertNotEqual(inspect.getargspec(testingFunc2), argspec)
        self.assertEqual(inspect.getargspec(testingFunc3), argspec)

        self.assertEqual(testingFunc1.__name__, 'testingFunc1')
        self.assertNotEqual(testingFunc2.__name__, 'testingFunc2')
        self.assertEqual(testingFunc3.__name__, 'testingFunc3')

if __name__ == '__main__':
    unittest.main()
