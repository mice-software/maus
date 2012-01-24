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

import json
import unittest
import MAUS
import ROOT

from celery import current_app
from celery.registry import tasks
from celery.utils import LOG_LEVELS

from mauscelery.maustasks import MausGenericTransformTask
from mauscelery.maustasks import get_maus_configuration
from mauscelery.maustasks import set_maus_configuration

from workers import WorkerBirthFailedException
from workers import WorkerDeathFailedException

class MapDummy: # pylint:disable = R0902
    """
    Simple map class for testing purposes.
    """

    def __init__(self):
        """
        Constructor.
        @param self Object reference.
        """
        # Flag holding result of death call.
        self.__death_result = True

    def birth(self, json_doc):
        """
        Birth the mapper. If the configuration doc has a key
        fail_birth then birth returns False. If it has a key
        fail_death then, when death is called, it will return
        False.
        @param self Object reference.
        @param json_doc JSON configuration document.
        @return False if fail_birth is in json_doc else return 
        True.
        """
        config = json.loads(json_doc)
        self.__death_result = not config.has_key("fail_death")
        return not config.has_key("fail_birth")

    def process(self, spill_doc): # pylint:disable = R0201
        """
        Process a spill and add a "processed" field.
        @param self Object reference.
        @param spill JSON spill document.
        @return updated spill document.
        """
        spill = json.loads(spill_doc)
        spill["processed"] = "processed"
        return json.dumps(spill)

    def death(self):
        """
        Death the mapper.
        @param self Object reference.
        @return True unless birth was given a configuration that
        requested this return False.
        """
        return self.__death_result

# Add MapDummy to MAUS so it can be picked up by the task when it
# needs to be created via its name.
setattr(MAUS, "MapDummy", MapDummy)

class MausGenericTransformTaskTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.maustasks.MausGenericTransformTask class.
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Create test object.
        @param self Object reference.
        """
        # Set Celery logging to ensure conditional log statements
        # are called.
        current_app.conf.CELERYD_LOG_LEVEL = LOG_LEVELS["INFO"]
        self.__generic_task = MausGenericTransformTask()

    def check_task_configuration(self, 
        configuration = "{}",
        transform = "MapPyDoNothing"):
        """ 
        Check the task configuration.
        @param self Object reference.
        @param configuration Expected configuration.
        @param transform Expected transform names.
        """
        self.assertEquals(configuration, 
                          self.__generic_task.configuration, 
                          "Unexpected configuration")
        self.assertEquals(transform, 
                          self.__generic_task.get_transform_names(), 
                          "Unexpected transform names")

    def test_init(self): # pylint: disable=R0201
        """ 
        Invoke __init__ and check default values.
        @param self Object reference.
        """
        self.check_task_configuration()
        self.assertTrue(ROOT.gROOT.IsBatch(), "Expected ROOT.IsBatch() to be True") # pylint:disable=E1101, C0301

    def test_run(self):
        """ 
        Invoke run with default worker.
        @param self Object reference.
        """
        spill = self.__generic_task.run("{}")
        self.assertEquals("{}", spill, "Unpexpected result spill")

    def test_reset_task_defaults(self):
        """ 
        Invoke reset_task with default arguments.
        @param self Object reference.
        """
        self.__generic_task.reset_task()
        # Expect no change in configuration.
        self.check_task_configuration()

    def test_reset_task_bad_config(self):
        """ 
        Invoke reset_task with a non-JSON configuration.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*Expecting object.*"):
            self.__generic_task.reset_task(configuration = "{")
        # Expect no change in configuration.
        self.check_task_configuration()

    def test_reset_task_bad_worker(self):
        """ 
        Invoke reset_task with an unknown transform.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*No such transform: UnknownTransform.*"):
            self.__generic_task.reset_task(transform = "UnknownTransform")
        # Expect no change in configuration.
        self.check_task_configuration()

    def test_reset_task(self):
        """ 
        Invoke reset_task.
        @param self Object reference.
        """
        configuration = """{}"""
        transform = "MapDummy"
        self.__generic_task.reset_task(
            configuration = configuration,
            transform = transform)
        # Expect change in configuration.
        self.check_task_configuration(configuration, transform)
        # Submit a spill and check MapDummy processes it.
        spill = self.__generic_task.run("{}")
        spill_doc = json.loads(spill)
        self.assertTrue(spill_doc.has_key("processed"), 
                        "Spill does not seem to have been processed")

    def test_reset_task_fail_death(self):
        """ 
        Invoke reset_task when the current transform fails during
        its death step.
        @param self Object reference.
        """
        configuration = """{"fail_death":"fail"}"""
        transform = "MapDummy"
        # Reset the task to be MapDummy and ensure that its death
        # call will fail.
        self.__generic_task.reset_task(
            configuration = configuration,
            transform = transform)
        # Now reset the task again, and see if the death call
        # does indeed fail.
        with self.assertRaisesRegexp(WorkerDeathFailedException,
            ".*MapDummy.*returned False.*"):
            self.__generic_task.reset_task(
                configuration = "{}",
                transform = "MapPyDoNothing")
        # Expect no change in configuration.
        self.check_task_configuration(configuration, transform)

    def test_reset_task_fail_birth(self):
        """ 
        Invoke reset_task when the new transform fails during its birth
        step.
        @param self Object reference.
        """
        configuration = """{"fail_birth":"fail"}"""
        transform = "MapDummy"
        # Reset the task to be MapDummy and ensure that its birth
        # call will fail.
        with self.assertRaisesRegexp(WorkerBirthFailedException,
            ".*MapDummy.*returned False.*"):
            self.__generic_task.reset_task(
                configuration = configuration,
                transform = transform)
        # Expect change in configuration.
        self.check_task_configuration(configuration, transform)

    def test_panel_get_config(self):
        """ 
        Test the get_maus_configuration panel operation.
        @param self Object reference.
        """
        maustask = \
            tasks["mauscelery.maustasks.MausGenericTransformTask"] 
        config = get_maus_configuration(None)
        self.assertTrue(config.has_key("configuration"), 
            "Missing configuration key")
        self.assertEquals(maustask.configuration, 
            config["configuration"], "Unexpected configuration")
        self.assertTrue(config.has_key("transform"),
            "Missing transform")
        self.assertEquals(maustask.get_transform_names(), 
            config["transform"], "Unexpected transform")

    def test_panel_set_config_defaults(self):
        """ 
        Test the set_maus_configuration panel operation with its
        default arguments.
        @param self Object reference.
        """
        maustask = \
            tasks["mauscelery.maustasks.MausGenericTransformTask"] 
        # Save current configuration.
        configuration = maustask.configuration
        transform = maustask.get_transform_names()
        # Invoke.
        result = set_maus_configuration(None)
        self.assertTrue(result.has_key("status"), 
            "Missing status key")
        self.assertEquals("ok", result["status"], "Unexpected status")
        # Expect no change in configuration.
        self.assertEquals(configuration, 
                          maustask.configuration, 
                          "Unexpected configuration")
        self.assertEquals(transform, 
                          maustask.get_transform_names(), 
                          "Unexpected transform names")

    def test_panel_set_config(self):
        """ 
        Test the set_maus_configuration panel operation.
        @param self Object reference.
        """
        maustask = \
            tasks["mauscelery.maustasks.MausGenericTransformTask"] 
        configuration = """{}"""
        transform = "MapDummy"
        result = set_maus_configuration(None, configuration, transform)
        self.assertTrue(result.has_key("status"), 
            "Missing status key")
        self.assertEquals("ok", result["status"], "Unexpected status")
        # Expect change in configuration.
        self.assertEquals(configuration, 
                          maustask.configuration, 
                          "Unexpected configuration")
        self.assertEquals(transform, 
                          maustask.get_transform_names(), 
                          "Unexpected transform names")

    def test_panel_set_config_error(self):
        """
        Test the set_maus_configuration panel operation where
        the task throws an exception.
        @param self Object reference.
        """
        maustask = \
            tasks["mauscelery.maustasks.MausGenericTransformTask"] 
        # Set configuration to be unicode to test conversion.
        configuration = """{"fail_birth":"fail"}""".decode()
        transform = "MapDummy"
        # Invoke.
        result = set_maus_configuration(None, configuration, transform)
        self.assertTrue(result.has_key("status"), 
            "Missing status key")
        self.assertEquals("error", result["status"], "Unexpected status")
        self.assertTrue(result.has_key("error"), 
            "Missing error key")
        self.assertTrue(result.has_key("message"), 
            "Missing message key")
        # Expect change in configuration.
        self.assertEquals(configuration, 
                          maustask.configuration, 
                          "Unexpected configuration")
        self.assertEquals(transform, 
                          maustask.get_transform_names(), 
                          "Unexpected transform names")

if __name__ == '__main__':
    unittest.main()
