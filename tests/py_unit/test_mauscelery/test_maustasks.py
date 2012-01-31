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
import os
import unittest
import MAUS
import ROOT

from celery import current_app
from celery.utils import LOG_LEVELS

from mauscelery.maustasks import MausConfiguration
from mauscelery.maustasks import MausTransform
from mauscelery.maustasks import worker_process_init_callback
from mauscelery.maustasks import execute_transform
from mauscelery.maustasks import process_birth
from mauscelery.maustasks import process_death
from mauscelery.maustasks import get_maus_configuration

from workers import WorkerBirthFailedException
from workers import WorkerDeathFailedException
from workers import WorkerDeadException

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

class MausTransformTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.maustasks.MausTransform class.
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

    def test_initialize(self):
        """ 
        Invoke initialize.
        @param self Object reference.
        """
        MausTransform.initialize("MapDummy")
        self.assertEquals(MapDummy, \
            MausTransform.transform.__class__, "Unexpected transform")
        self.assertTrue(MausTransform.is_dead, 
            "Expected is_dead to be True")
        self.assertTrue(ROOT.gROOT.IsBatch(), "Expected ROOT.IsBatch() to be True") # pylint:disable=E1101, C0301

    def test_initialize_bad_transform(self):
        """ 
        Invoke initialize with an unknown transform.
        @param self Object reference.
        """
        with self.assertRaisesRegexp(ValueError,
            ".*No such transform: UnknownTransform.*"):
            MausTransform.initialize("UnknownTransform")
        self.assertEquals(None, MausTransform.transform, \
            "Unexpected transform")
        self.assertTrue(MausTransform.is_dead, \
            "Expected is_dead to be True")

    def test_birth(self):
        """ 
        Invoke birth
        @param self Object reference.
        """
        MausTransform.initialize("MapDummy")
        MausTransform.birth("{}")
        self.assertTrue(not MausTransform.is_dead, \
            "Expected is_dead to be False")

    def test_birth_fails(self):
        """ 
        Invoke birth when WorkerBirthFailedException is thrown.
        @param self Object reference.
        """
        MausTransform.initialize("MapDummy")
        with self.assertRaisesRegexp(WorkerBirthFailedException,
            ".*MapDummy returned False.*"):
            MausTransform.birth("""{"fail_birth":"true"}""")

    def test_process(self):
        """ 
        Invoke process.
        @param self Object reference.
        """
        MausTransform.initialize("MapDummy")
        MausTransform.birth("{}")
        spill = MausTransform.process("{}")
        spill_doc = json.loads(spill)
        self.assertTrue(spill_doc.has_key("processed"),
            "Expected spill to have been processed")

    def test_process_after_death(self):
        """ 
        Invoke death when WorkerDeathFailedException is thrown.
        @param self Object reference.
        """
        MausTransform.initialize("MapDummy")
        MausTransform.birth("{}")
        MausTransform.death()
        with self.assertRaisesRegexp(WorkerDeadException,
            ".*MapDummy process called after death.*"):
            MausTransform.process("{}")

    def test_death(self):
        """ 
        Invoke death.
        @param self Object reference.
        """
        MausTransform.initialize("MapDummy")
        MausTransform.birth("{}")
        MausTransform.death()
        self.assertTrue(MausTransform.is_dead, \
            "Expected is_dead to be True")

    def test_death_fails(self):
        """ 
        Invoke death when WorkerDeathFailedException is thrown.
        @param self Object reference.
        """
        MausTransform.initialize("MapDummy")
        MausTransform.birth("""{"fail_death":"true"}""")
        with self.assertRaisesRegexp(WorkerDeathFailedException,
            ".*MapDummy returned False.*"):
            MausTransform.death()

class WorkerProcessInitCallbackTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.maustasks.worker_process_init_callback 
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
        MausConfiguration.transform = "MapDummy"
        MausConfiguration.configuration = "{}"
        worker_process_init_callback()
        self.assertEquals(MapDummy, \
            MausTransform.transform.__class__, "Unexpected transform")
        self.assertTrue(not MausTransform.is_dead, 
            "Expected is_dead to be False")

class ExecuteTransformTaskTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.maustasks.execute_transform method. 
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
 
    def test_execute_transform(self):
        """ 
        Invoke execute_transform.
        @param self Object reference.
        """
        MausTransform.initialize("MapDummy")
        MausTransform.birth("{}")
        spill = execute_transform("{}")
        spill_doc = json.loads(spill)
        self.assertTrue(spill_doc.has_key("processed"),
            "Expected spill to have been processed")

class ProcessBirthTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.maustasks.process_birth method. 
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
            "MapDummy", "{}")
        self.assertEquals(os.getpid(), status[0], 
            "Unexpected process ID")
        self.assertEquals(None, status[1], "Expected None")

    def test_process_birth(self):
        """ 
        Invoke process_birth.
        @param self Object reference.
        """
        new_id = MausConfiguration.config_id + 1
        status = process_birth(new_id, "MapDummy", "{}")
        self.assertEquals(MapDummy, \
            MausTransform.transform.__class__, "Unexpected transform")
        self.assertTrue(not MausTransform.is_dead, 
            "Expected is_dead to be False")
        self.assertEquals("MapDummy", MausConfiguration.transform,
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
            "MapDummy", """{"fail_birth":"true"}""")
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
    Test class for mauscelery.maustasks.process_death method. 
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
            "MapDummy", "{}")
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
            "MapDummy", "{}")
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
            "MapDummy", """{"fail_death":"true"}""")
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

class GetMausConfigPanelTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.maustasks.get_maus_configuration method. 
    """

    def test_panel_get_config(self):
        """ 
        Test the get_maus_configuration panel operation.
        @param self Object reference.
        """
        MausConfiguration.transform = "MapDummy"
        MausConfiguration.configuration = "{test}"
        config = get_maus_configuration(None)
        self.assertTrue(config.has_key("config_id"), 
            "Missing config_id key")
        self.assertEquals(MausConfiguration.config_id, 
            config["config_id"], "Unexpected config_id")
        self.assertTrue(config.has_key("configuration"), 
            "Missing configuration key")
        self.assertEquals(MausConfiguration.configuration, 
            config["configuration"], "Unexpected configuration")
        self.assertTrue(config.has_key("transform"),
            "Missing transform")
        self.assertEquals(MausConfiguration.transform,
            config["transform"], "Unexpected transform")

if __name__ == '__main__':
    unittest.main()
