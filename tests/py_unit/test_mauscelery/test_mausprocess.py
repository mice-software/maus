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

import logging
import os
import unittest
import json

import maus_cpp.globals
import Configuration
import mauscelery.mausprocess
from framework.workers import WorkerBirthFailedException
from mauscelery.state import MausConfiguration
from mauscelery.state import MausTransform
from mauscelery.mausprocess import worker_process_init_callback
from mauscelery.mausprocess import process_birth
from mauscelery.mausprocess import process_death
from MapPyTestMap import MapPyTestMap

DEFAULT_CONFIG = Configuration.Configuration().getConfigJSON()
def init_logging():
    """
    Set logging to verbose mode if I am debugging
    """
    logging.basicConfig()
    logger = logging.getLogger(mauscelery.mausprocess.__name__)
    logger.setLevel(logging.DEBUG)
    logger = logging.getLogger(mauscelery.state.__name__)
    logger.setLevel(logging.DEBUG)
    logger = logging.getLogger(MausTransform.__name__)
    logger.setLevel(logging.DEBUG)
    logger = logging.getLogger(MausConfiguration.__name__)
    logger.setLevel(logging.DEBUG)

class ProcessInitialiseTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
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
        MausConfiguration.transform = "MapPyTestMap"
        # Configure lowest logging level so all logging statements
        # are executed.

    def test_callback_no_globals(self):
        """ 
        Invoke worker_process_init_callback - check globals are not created with
        empty configuration
        """
        MausConfiguration.configuration = "{}"
        if maus_cpp.globals.has_instance():
            maus_cpp.globals.death()
        # doesnt initialise globals with configuration {}
        self.assertRaises(WorkerBirthFailedException,
                          worker_process_init_callback)
        self.assertFalse(maus_cpp.globals.has_instance())
        self.assertEquals(MapPyTestMap, MausTransform.transform.__class__)
        
    def test_process_defaults(self):
        """ 
        Check worker_process_init_callback constructs globals okay
        """
        MausConfiguration.configuration = DEFAULT_CONFIG
        # initialises globals with reasonable configuration
        worker_process_init_callback()
        # reinitialised globals
        worker_process_init_callback()
        self.assertTrue(maus_cpp.globals.has_instance())
        self.assertEquals(MapPyTestMap, MausTransform.transform.__class__)

class ProcessBirthTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.mausprocess.process_birth method. 
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Reset MausTransform and set logging.
        @param self Object reference.
        """
        MausConfiguration.configuration = DEFAULT_CONFIG
        worker_process_init_callback()
        MausTransform.transform = None
        MausTransform.is_dead = True
        # Configure lowest logging level so all logging statements
        # are executed.
 
    def test_process_birth_same_id(self):
        """ 
        Invoke process_birth with a config ID that matches
        the current ID (in MausConfiguration.config_id).
        @param self Object reference.
        """
        status = process_birth(set(), MausConfiguration.config_id, \
            "MapPyTestMap", DEFAULT_CONFIG, 0)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertEquals(None, status[1], "Expected None")

    def test_process_birth(self):
        """ 
        Invoke process_birth.
        @param self Object reference.
        """
        new_id = MausConfiguration.config_id + 1
        status = process_birth(set(), new_id, "MapPyTestMap", DEFAULT_CONFIG, 9)
        self.assertEquals(MapPyTestMap, \
            MausTransform.transform.__class__, "Unexpected transform")
        self.assertTrue(not MausTransform.is_dead, 
            "Expected is_dead to be False")
        self.assertEquals("MapPyTestMap", MausConfiguration.transform,
            "Unexpected MausConfiguration.transform")
        self.assertEquals(DEFAULT_CONFIG, MausConfiguration.configuration,
            "Unexpected MausConfiguration.configuration")
        self.assertEquals(new_id, MausConfiguration.config_id,
            "Unexpected MausConfiguration.config_id")
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertEquals(None, status[1], "Expected None")
        self.assertEquals(9, json.loads(status[2])["run_number"])

    def test_process_birth_fails(self):
        """ 
        Invoke process_birth where birth throws a
      . WorkerBirthFailedException.
        @param self Object reference.
        """
        status = process_birth(set(), MausConfiguration.config_id + 1, 
            "MapPyTestMap", """{"birth_result":%s}""" % \
            MapPyTestMap.FAIL, 0)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertTrue(status[1].has_key("error"),
            "Expect an error key")
        self.assertTrue(status[1].has_key("message"),
            "Expect a message key")

    def test_process_birth_exception(self):
        """ 
        Invoke process_birth where birth throws an exception.
        @param self Object reference.
        """
        status = process_birth(set(), MausConfiguration.config_id + 1, 
            "MapPyTestMap", """{"birth_result":%s}""" % \
            MapPyTestMap.EXCEPTION, 0)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertTrue(status[1].has_key("error"),
            "Expect an error key")
        self.assertTrue(status[1].has_key("message"),
            "Expect a message key")

    def test_process_death_fails(self):
        """ 
        Invoke process_birth where death of existing transform throws
        an exception.
        @param self Object reference.
        """
        process_birth(set(), MausConfiguration.config_id + 1, 
            "MapPyTestMap", """{"death_result":%s}""" % \
            MapPyTestMap.EXCEPTION, 0)
        # Now try knowing that death will fail.
        status = process_birth(set(), MausConfiguration.config_id + 2, 
            "MapPyTestMap", DEFAULT_CONFIG, 0)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertTrue(status[1].has_key("error"),
            "Expect an error key")
        self.assertTrue(status[1].has_key("message"),
            "Expect a message key")
        self.assertTrue(MausTransform.is_dead, 
            "Expected is_dead to be True")
        # Now try again - should be fine.
        status = process_birth(set(), MausConfiguration.config_id + 3, 
            "MapPyDoNothing", DEFAULT_CONFIG, 0)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID on subsequent birth call")
        self.assertEquals(None, status[1], 
            "Unexpected status value on subsequent birth call")
        self.assertTrue(not MausTransform.is_dead, 
            "Expected is_dead to be False after subsequent birth call")
        self.assertEquals("MapPyDoNothing", MausConfiguration.transform,
            "Unexpected MausConfiguration.transform")

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
        MausConfiguration.configuration = DEFAULT_CONFIG
        worker_process_init_callback() # start the worker
        # Configure lowest logging level so all logging statements
        # are executed.
        logger = logging.getLogger(mauscelery.mausprocess.__name__)
        logger.setLevel(logging.DEBUG)
        logger = logging.getLogger(mauscelery.state.__name__)
        logger.setLevel(logging.DEBUG)
 
    def test_process_death(self):
        """ 
        Invoke process_death.
        @param self Object reference.
        """
        process_birth(set(), MausConfiguration.config_id + 1, 
            "MapPyTestMap", DEFAULT_CONFIG, -1)
        status = process_death(set(), -69)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertEquals(None, status[1],
            "Unexpected status value")
        self.assertEquals(-69, json.loads(status[2])["run_number"])
        self.assertTrue(MausTransform.is_dead, 
            "Expected is_dead to be True")
        # Expect same again. Doesn't call death()/end_of_run() twice
        status = process_death(set(), -69)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID on subseqent death call")
        self.assertEquals(None, status[1],
            "Unexpected status value on subsequent death call")
        self.assertEquals('', status[2],
            "Unexpected run_number on subsequent death call "+status[2])
        self.assertTrue(MausTransform.is_dead, 
            "Expected is_dead to be True after subsequent death call")

    def test_process_death_exception(self):
        """ 
        Invoke process_death where death throws an exception.
        @param self Object reference.
        """
        
        status = process_birth(set(), MausConfiguration.config_id + 1, 
            "MapPyTestMap", """{"death_result":%s}""" % \
            MapPyTestMap.EXCEPTION, 0)
        status = process_death(set(), 0)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertTrue(status[1].has_key("error"),
            "Expect an error key")
        self.assertTrue(status[1].has_key("message"),
            "Expect a message key")
        # Expect subsequent attempt to succeed.
        status = process_death(set(), 0)
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID on subseqent death call")
        self.assertEquals(None, status[1],
            "Unexpected status value on subsequent death call")
        self.assertTrue(MausTransform.is_dead, 
            "Expected is_dead to be True after subsequent death call")

if __name__ == '__main__':
    #init_logging()
    unittest.main()
