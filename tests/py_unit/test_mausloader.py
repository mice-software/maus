"""
A custom loader for Celery which ensures that all tasks available
to workers are initialised with MAUS configuration.
Test class for mausloader.TaskBirthException module.
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

from celery.registry import tasks
from celery.task import Task

from mauscelery.mausloader import CeleryLoader
from mauscelery.mausloader import TaskBirthException

class TestCeleryTask(Task):
    """
    Simple Celery task for testing purposes.
    """

    # Control behaviour of birth.
    behaviour = ""

    def __init__(self):
        """
        Constructor. No-op.
        @param self Object reference.
        """
        pass

    def birth(self, json_config_doc): # pylint: disable=R0201, W0613
        """
        If behaviour has value "False" then return False.
        If behaviour has value "Exception" then raise an Exception.
        Else, return True.
        @param self Object reference.
        @param json_config_doc JSON configuration document.       
        @return result of invoking birth.
        @throws exception if any exceptions arise in birth().
        """
        if (TestCeleryTask.behaviour == "False"):
            return False
        if (TestCeleryTask.behaviour == "Exception"):
            raise Exception("BirthError")
        return True

    def run(self): # pylint: disable=R0201
        """
        @return True always.
        """
        return True

# Running test class via nosetests gives test_mausloader.TestCeleryTask
# Running test class directly via python gives default.TestCeleryTask
# So, explictly set the module and class name.
TestCeleryTask.name = "test_mausloader.TestCeleryTask"
tasks.register(TestCeleryTask) # pylint:disable=W0104, E1101

class CeleryLoaderTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.mausloader module.
    """

    def setUp(self):
        """ 
        Get default task and set configuration value.
        @param self Object reference.
        """
        self.__task_name = TestCeleryTask.name
        self.__loader = CeleryLoader()
        self.__task = tasks[self.__task_name]
        TestCeleryTask.behaviour = ""

    def test_on_worker_init(self):
        """ 
        Invoke on_worker_init with default values.
        @param self Object reference.
        """
        self.__loader.on_worker_init()

    def test_init_birth_exception(self):
        """ 
        Invoke on_worker_init using a task which throws an exception
        when birth is invoked.
        @param self Object reference.
        """
        TestCeleryTask.behaviour = "Exception"
        with self.assertRaisesRegexp(TaskBirthException,
            ".*TestCeleryTask.birth failed due to: BirthError.*"):
            self.__loader.on_worker_init()

    def test_init_birth_false(self):
        """ 
        Invoke on_worker_init using a task which returns false when
        birth is invoked.
        @param self Object reference.
        """
        TestCeleryTask.behaviour = "False"
        with self.assertRaisesRegexp(TaskBirthException,
            ".*TestCeleryTask.birth returned False.*"):
            self.__loader.on_worker_init()

class TaskBirthExceptionTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.TaskBirthException module.
    """

    def test_init_default(self):
        """ 
        Invoke default constructor and check default values.
        @param self Object reference.
        """
        exception = TaskBirthException("TASK")
        self.assertEquals("TASK.birth returned False",
                          str(exception), 
                          "Unexpected string")
        self.assertEquals(exception.task, "TASK")
        self.assertEquals(exception.cause, None)

    def test_init(self):
        """ 
        Invoke constructor and check values.
        @param self Object reference.
        """
        cause = Exception("CAUSE")
        exception = TaskBirthException("TASK", cause)
        self.assertEquals("TASK.birth failed due to: %s" % cause,
                          str(exception), 
                          "Unexpected string")
        self.assertEquals(exception.task, "TASK")
        self.assertEquals(exception.cause, cause)

if __name__ == '__main__':
    unittest.main()
