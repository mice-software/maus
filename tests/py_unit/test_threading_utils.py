#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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

"""
Test threading utils
"""

import Queue
import unittest

from threading_utils import ThreadedBool
from threading_utils import ThreadedValue
from threading_utils import generate_fifo_queue
from threading_utils import go_back_fifo_queue

class ThreadedBoolTest(unittest.TestCase): # pylint: disable=R0904
    """
    Test threaded bool
    """
    def test_threaded_bool(self):
        """
        Test threaded bool
        """
         # duplicate lines are intended, leave them in the test
        _true = ThreadedBool(1)
        self.assertTrue(_true)
        self.assertTrue(_true)
        _false = ThreadedBool(None)
        self.assertFalse(_false)
        self.assertFalse(_false)
        _test = _true
        _test.set_false()
        self.assertFalse(_test)
        _test.set_false()
        self.assertFalse(_test)
        _test.set_true()
        self.assertTrue(_test)
        _test.set_true()
        self.assertTrue(_test)

class ThreadedValueTest(unittest.TestCase): # pylint: disable=R0904
    """
    Test threaded value
    """
    def test_threaded_value(self):
        """
        Test threaded value
        """
         # duplicate lines are intended, leave them in the test
        _value = ThreadedValue(1)
        self.assertEqual(_value.get_value(), 1)
        self.assertEqual(_value.get_value(), 1)
        _value.set_value(2)
        self.assertEqual(_value.get_value(), 2)
        self.assertEqual(_value.get_value(), 2)
        _value.set_value(3)
        self.assertEqual(_value.get_value(), 3)
        self.assertEqual(_value.get_value(), 3)

class QueueUtilsTest(unittest.TestCase): # pylint: disable=R0904
    """
    Test Queueutils
    """
    def test_generate_fifo_queue(self):
        """
        Test threaded bool
        """
        test_queue = Queue.Queue()
        for i in range(10):
            test_queue.put(i)
        # check data is correct
        generator_output = [i for i in generate_fifo_queue(test_queue)]
        self.assertEqual(generator_output, range(10))
        # check ordering is not changed
        generator_output = [i for i in generate_fifo_queue(test_queue)]
        self.assertEqual(generator_output, range(1, 10)+[0])
        last = test_queue.get_nowait()
        test_queue.put(last)
        go_back_fifo_queue(test_queue)
        self.assertEqual(test_queue.get_nowait(), last)

if __name__ == "__main__":
    unittest.main()
