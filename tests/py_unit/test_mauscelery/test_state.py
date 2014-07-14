"""
Test class for mauscelery.state module.
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
import logging
import unittest
import ROOT

import Configuration
import maus_cpp.globals
from framework.workers import WorkerBirthFailedException
from framework.workers import WorkerDeathFailedException
from framework.workers import WorkerDeadException
import mauscelery.state
from mauscelery.state import MausTransform
from MapPyTestMap import MapPyTestMap

class MausTransformTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.state.MausTransform class.
    """

    def setUp(self): # pylint: disable=C0103
        """ 
        Reset MausTransform and set logging.
        @param self Object reference.
        """
        if not maus_cpp.globals.has_instance():
            maus_cpp.globals.birth(
                                  Configuration.Configuration().getConfigJSON())
        MausTransform.transform = None
        MausTransform.is_dead = True
        # Configure lowest logging level so all logging statements
        # are executed.
        logger = logging.getLogger(mauscelery.state.__name__)
        logger.setLevel(logging.DEBUG)

    def test_initialize(self):
        """ 
        Invoke initialize.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        self.assertEquals(MapPyTestMap, \
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
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("{}")
        self.assertTrue(not MausTransform.is_dead, \
            "Expected is_dead to be False")

    def test_birth_fails(self):
        """ 
        Invoke birth when WorkerBirthFailedException is thrown.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        with self.assertRaisesRegexp(WorkerBirthFailedException,
            ".*MapPyTestMap returned False.*"):
            MausTransform.birth("""{"birth_result":%s}""" % \
                MapPyTestMap.FAIL)

    def test_birth_exception(self):
        """ 
        Invoke birth when a ValueError is thrown by birth.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        with self.assertRaisesRegexp(WorkerBirthFailedException,
            ".*Birth exception.*"):
            MausTransform.birth("""{"birth_result":%s}""" % \
                MapPyTestMap.EXCEPTION)

    def test_process(self):
        """ 
        Invoke process.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("{}")
        spill = MausTransform.process("{}")
        spill_doc = json.loads(spill)
        self.assertTrue(spill_doc.has_key("processed"),
            "Expected spill to have been processed")

    def test_process_exception(self):
        """ 
        Invoke process when a ValueError is thrown by process.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("""{"process_result":%s}""" % \
            MapPyTestMap.EXCEPTION)
        with self.assertRaisesRegexp(ValueError,
            ".*Process exception.*"):
            MausTransform.process("{}")

    def test_process_after_death(self):
        """ 
        Invoke death when WorkerDeathFailedException is thrown.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("{}")
        MausTransform.death()
        with self.assertRaisesRegexp(WorkerDeadException,
            ".*MapPyTestMap process called after death.*"):
            MausTransform.process("{}")

    def test_death(self):
        """ 
        Invoke death.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("{}")
        MausTransform.death()
        self.assertTrue(MausTransform.is_dead, \
            "Expected is_dead to be True")

    def test_death_fails(self):
        """ 
        Invoke death when WorkerDeathFailedException is thrown.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("""{"death_result":%s}""" % \
            MapPyTestMap.FAIL)
        with self.assertRaisesRegexp(WorkerDeathFailedException,
            ".*MapPyTestMap returned False.*"):
            MausTransform.death()

    def test_death_exception(self):
        """ 
        Invoke death when a ValueError is thrown by death.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("""{"death_result":%s}""" % \
            MapPyTestMap.EXCEPTION)
        with self.assertRaisesRegexp(WorkerDeathFailedException,
            ".*Death exception.*"):
            MausTransform.death()
        self.assertTrue(MausTransform.is_dead, 
            "Expected is_dead to be True")

    def test_death_fails_birth(self):
        """ 
        Test birth when current transform death returns False.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("""{"death_result":%s}""" % \
            MapPyTestMap.FAIL)
        with self.assertRaisesRegexp(WorkerDeathFailedException,
            ".*MapPyTestMap returned False.*"):
            MausTransform.birth("""{"death_result":%s}""" % \
                MapPyTestMap.FAIL)
        # Except success on second attempt.
        MausTransform.birth("{}")

    def test_initialize_fails_birth(self):
        """ 
        Test initialize when current transform death returns False.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("""{"death_result":%s}""" % \
            MapPyTestMap.FAIL)
        with self.assertRaisesRegexp(WorkerDeathFailedException,
            ".*MapPyTestMap returned False.*"):
            MausTransform.initialize("MapPyTestMap")
        # Except success on second attempt.
        MausTransform.initialize("MapPyTestMap")

if __name__ == '__main__':
    unittest.main()
