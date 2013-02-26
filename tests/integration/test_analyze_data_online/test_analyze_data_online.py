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
test_analyze_online.py - Note that I never ran this test successfully! Always
skips until someone sorts out the environment on the test server.
"""

import os
import subprocess
import signal
import unittest
import time

ONLINE_TEST = os.path.expandvars(
                            "$MAUS_ROOT_DIR/tests/integration/"+\
                            "test_distributed_processing/_test_online_okay.py")
ANALYZE_EXE = os.path.expandvars("$MAUS_ROOT_DIR/bin/analyze_data_online.py")
TMP_DIR =  os.path.expandvars("$MAUS_ROOT_DIR/tmp/")
LOG = TMP_DIR+"test_analyze_data_online.log"

def online_okay():
    """Return true if online libraries are okay"""
    log = open("/dev/null", "w")
    proc = subprocess.Popen(["python", ONLINE_TEST], stdout=log,
                                                     stderr=subprocess.STDOUT)
    proc.wait() # pylint: disable=E1101
    return proc.returncode == 0 # pylint: disable=E1101

class TestAnalyzeOnline(unittest.TestCase):#pylint: disable =R0904
    """Execute analyze_data_online - crash out if it doesnt work"""
    def test_analyze_online(self):
        """test_analyze_online"""
        if not online_okay():
            unittest.TestCase.skipTest(self, 
                                       "Skip - online services are not running")
        log = open(LOG, "w")
        print "running analyze online"
        env_cp = os.environ.copy()
        env_cp['MAUS_WEB_MEDIA_RAW'] = TMP_DIR
        env_cp['MAUS_WEB_DIR'] = TMP_DIR
        proc = subprocess.Popen(['python', ANALYZE_EXE], env=env_cp,
                                           stdout=log, stderr=subprocess.STDOUT)
        time.sleep(10)
        print "killing analyze online", proc.returncode # pylint: disable=E1101
        self.assertEquals(proc.returncode, None) # pylint: disable=E1101
        proc.send_signal(signal.SIGINT) # pylint: disable=E1101
        proc.wait() # pylint: disable=E1101
        print "killed analyze online"
        self.assertEquals(proc.returncode, 0) # pylint: disable=E1101

if __name__ == "__main__":
    unittest.main()
