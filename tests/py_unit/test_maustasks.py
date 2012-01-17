"""
Test class for mauscelery.maustasks module.
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

# pylint: disable=C0103

import unittest

from mauscelery.maustasks import MausGenericTransformTask

class MausGenericTransformTaskTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.maustasks module.
    """

    def setUp(self):
        """ 
        Create test object.
        @param self Object reference.
        """
        self.__generic_task = MausGenericTransformTask()

    def test_init(self): # pylint: disable=R0201
        """ 
        Invoke __init__.
        @param self Object reference.
        """
        MausGenericTransformTask()

    def test_run_workers_none(self):
        """ 
        Invoke run with a worker list of None.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*None is not a list.*"):
            self.__generic_task.run(None, "{}")

    def test_run_workers_non_list(self):
        """ 
        Invoke run with a worker list that is not a ListType.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*non-list is not a list.*"):
            self.__generic_task.run("non-list", "{}")

    def test_run_workers_no_such_worker(self):
        """ 
        Invoke run with a worker list that has a non-existant worker.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*No such worker: NonExistant.*"):
            self.__generic_task.run(["MapPyDoNothing", "NonExistant"], "{}")

    def test_run_workers_non_string_worker(self):
        """ 
        Invoke run with a worker list that has a non-string worker name.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Worker name 123 is not a string.*"):
            self.__generic_task.run(["MapPyDoNothing", 123], "{}")

    def test_run_workers_empty(self):
        """ 
        Invoke run with an empty worker list.
        @param self Object reference.
        """
        spill = """{"sample_spill": "value"}"""
        result = self.__generic_task.run([], spill)
        # Expect a no-op.
        self.assertEquals(spill, result, "Unexpected result")

    def test_run_workers_solo_worker(self):
        """ 
        Invoke run with a single MapPyDoNothing worker.
        @param self Object reference.
        """
        spill = """{"sample_spill": "value"}"""
        result = self.__generic_task.run(["MapPyDoNothing"], spill)
        # Expect a no-op.
        self.assertEquals(spill, result, "Unexpected result")

    def test_run_workers_nested_worker(self):
        """ 
        Invoke run with a single MapPyDoNothing worker.
        @param self Object reference.
        """
        spill = """{"sample_spill": "value"}"""
        result = self.__generic_task.run(["MapPyDoNothing", 
            ["MapPyDoNothing", "MapPyDoNothing", ["MapPyDoNothing"]]], spill)
        # Expect a no-op.
        self.assertEquals(spill, result, "Unexpected result")

if __name__ == '__main__':
    unittest.main()
