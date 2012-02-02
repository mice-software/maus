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
import unittest
import ROOT

from celery import current_app
from celery.utils import LOG_LEVELS

from mauscelery.state import MausTransform

from workers import WorkerBirthFailedException
from workers import WorkerDeathFailedException
from workers import WorkerDeadException

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
            MausTransform.birth("""{"fail_birth":"true"}""")

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
        MausTransform.birth("""{"fail_death":"true"}""")
        with self.assertRaisesRegexp(WorkerDeathFailedException,
            ".*MapPyTestMap returned False.*"):
            MausTransform.death()

if __name__ == '__main__':
    unittest.main()
