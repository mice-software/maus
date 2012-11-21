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

"""
Check that celery is alive on the test server. Note that this should not be run
as part of the regular test script - only should be run by online machine (and
online-capable machines)
"""

import unittest
import celery.task.control
import pymongo
import pymongo.errors

class OnlineOkayTest(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Check that celery nodes are available and RabbitMQ is alive or fail
    """
    def test_celery_is_alive(self):
        """
        _test_online_okay: Check that celery can be accessed 

        Fail an assertion if celery.task.control.inspect() fails (RabbitMQ is
        down) or no active nodes
        """
        try:
            active_nodes = celery.task.control.inspect().active()
        except: #pylint: disable=W0702
            self.assertTrue(False, "Failed to inspect celery workers")
        self.assertNotEqual(active_nodes, None)

    def test_mongodb_is_alive(self):
        """
        _test_online_okay: Check that mongodb can be accessed

        Fail an assertion if mongodb can't be contacted on localhost        
        """
        try:
            test_connx = pymongo.Connection("localhost", 27017)
        except pymongo.errors.AutoReconnect: # pylint: disable=W0702
            self.assertTrue(False, "MongoDB server is not accessible")
        test_connx.disconnect()

if __name__ == "__main__":
    unittest.main()

