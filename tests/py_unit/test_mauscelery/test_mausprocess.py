"""
Test class for mauscelery.mausprocess module.
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

import os
import unittest

from celery import current_app
from celery.utils import LOG_LEVELS

from mauscelery.state import MausConfiguration
from mauscelery.state import MausTransform
from mauscelery.mausprocess import worker_process_init_callback
from mauscelery.mausprocess import process_birth
from mauscelery.mausprocess import process_death

from MapPyTestMap import MapPyTestMap

class WorkerProcessInitCallbackTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.mausprocess.worker_process_init_callback 
    method.
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Reset MausTransform and set logging.
        @param self Object reference.
        """
        MausTransform.transform = None
        MausTransform.is_dead = True
        # Set Celery logging to ensure conditional log statements
        # are called.
        current_app.conf.CELERYD_LOG_LEVEL = LOG_LEVELS["INFO"]
 
    def test_callback(self):
        """ 
        Invoke worker_process_init_callback.
        @param self Object reference.
        """
        MausConfiguration.transform = "MapPyTestMap"
        MausConfiguration.configuration = "{}"
        worker_process_init_callback()
        self.assertEquals(MapPyTestMap, \
            MausTransform.transform.__class__, "Unexpected transform")
        self.assertTrue(not MausTransform.is_dead, 
            "Expected is_dead to be False")

class ProcessBirthTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.mausprocess.process_birth method. 
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Reset MausTransform and set logging.
        @param self Object reference.
        """
        MausTransform.transform = None
        MausTransform.is_dead = True
        # Set Celery logging to ensure conditional log statements
        # are called.
        current_app.conf.CELERYD_LOG_LEVEL = LOG_LEVELS["DEBUG"]
 
    def test_process_birth_same_id(self):
        """ 
        Invoke process_birth with a config ID that matches
        the current ID (in MausConfiguration.config_id).
        @param self Object reference.
        """
        status = process_birth(MausConfiguration.config_id, \
            "MapPyTestMap", "{}")
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertEquals(None, status[1], "Expected None")

    def test_process_birth(self):
        """ 
        Invoke process_birth.
        @param self Object reference.
        """
        new_id = MausConfiguration.config_id + 1
        status = process_birth(new_id, "MapPyTestMap", "{}")
        self.assertEquals(MapPyTestMap, \
            MausTransform.transform.__class__, "Unexpected transform")
        self.assertTrue(not MausTransform.is_dead, 
            "Expected is_dead to be False")
        self.assertEquals("MapPyTestMap", MausConfiguration.transform,
            "Unexpected MausConfiguration.transform")
        self.assertEquals("{}", MausConfiguration.configuration,
            "Unexpected MausConfiguration.configuration")
        self.assertEquals(new_id, MausConfiguration.config_id,
            "Unexpected MausConfiguration.config_id")
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertTrue(status[1].has_key("status"),
            "Expect a status key")
        self.assertEquals("ok", status[1]["status"],
            "Expect a status key with value ok")

    def test_process_birth_fails(self):
        """ 
        Invoke process_birth where birth throws an exception.
        @param self Object reference.
        """
        status = process_birth(MausConfiguration.config_id + 1, 
            "MapPyTestMap", """{"birth_result":%s}""" % \
            MapPyTestMap.FAIL)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertTrue(status[1].has_key("status"),
            "Expect a status key")
        self.assertEquals("error", status[1]["status"],
            "Expect a status key with value error")
        self.assertTrue(status[1].has_key("type"),
            "Expect a type key")
        self.assertTrue(status[1].has_key("message"),
            "Expect a message key")

class ProcessDeathTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.mausprocess.process_death method. 
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Reset MausTransform and set logging.
        @param self Object reference.
        """
        MausTransform.transform = None
        MausTransform.is_dead = True
        # Set Celery logging to ensure conditional log statements
        # are called.
        current_app.conf.CELERYD_LOG_LEVEL = LOG_LEVELS["DEBUG"]
 
    def test_process_death(self):
        """ 
        Invoke process_death.
        @param self Object reference.
        """
        process_birth(MausConfiguration.config_id + 1, 
            "MapPyTestMap", "{}")
        status = process_death()
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertTrue(status[1].has_key("status"),
            "Expect a status key")
        self.assertEquals("ok", status[1]["status"],
            "Expect a status key with value ok")

    def test_process_death_already_dead(self):
        """ 
        Invoke process_death when the transform is already dead.
        @param self Object reference.
        """
        process_birth(MausConfiguration.config_id + 1, 
            "MapPyTestMap", "{}")
        process_death()
        status = process_death()
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertEquals(None, status[1], "Expected None")

    def test_process_death_fails(self):
        """ 
        Invoke process_birth where birth throws an exception.
        @param self Object reference.
        """
        status = process_birth(MausConfiguration.config_id + 1, 
            "MapPyTestMap", """{"death_result":%s}""" % \
            MapPyTestMap.FAIL)
        status = process_death()
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertTrue(status[1].has_key("status"),
            "Expect a status key")
        self.assertEquals("error", status[1]["status"],
            "Expect a status key with value error")
        self.assertTrue(status[1].has_key("type"),
            "Expect a type key")
        self.assertTrue(status[1].has_key("message"),
            "Expect a message key")

if __name__ == '__main__':
    unittest.main()
