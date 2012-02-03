"""
Test class for mauscelery.tasks module.
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

from celery import current_app
from celery.utils import LOG_LEVELS

from mauscelery.state import MausTransform
from mauscelery.tasks import execute_transform
from MapPyTestMap import MapPyTestMap

class ExecuteTransformTaskTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for mauscelery.tasks.execute_transform method. 
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
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("{}")
        spill = execute_transform("{}")
        spill_doc = json.loads(spill)
        self.assertTrue(spill_doc.has_key("processed"),
            "Expected spill to have been processed")

    def test_execute_transform_fails(self):
        """ 
        Invoke execute_transform when a ValueError is thrown by
        the transform's process method.
        @param self Object reference.
        """
        MausTransform.initialize("MapPyTestMap")
        MausTransform.birth("""{"process_result":%s}""" % \
            MapPyTestMap.EXCEPTION)
        with self.assertRaisesRegexp(ValueError,
            ".*Process exception.*"):
            MausTransform.process("{}")

if __name__ == '__main__':
    unittest.main()
