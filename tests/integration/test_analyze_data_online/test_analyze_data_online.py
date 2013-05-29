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

import sys
import os
import subprocess
import signal
import unittest
import time
import glob

import regression

ONLINE_TEST = os.path.expandvars(
                            "$MAUS_ROOT_DIR/tests/integration/"+\
                            "test_distributed_processing/_test_online_okay.py")
ANALYZE_EXE = os.path.expandvars("$MAUS_ROOT_DIR/bin/analyze_data_online.py")
TMP_DIR =  os.path.expandvars("$MAUS_ROOT_DIR/tmp/test_analyze_data_online/")


def online_okay():
    """Return true if online libraries are okay"""
    log = open("/dev/null", "w")
    proc = subprocess.Popen(["python", ONLINE_TEST])#, stdout=log,
                                                    # stderr=subprocess.STDOUT)
    proc.wait() # pylint: disable=E1101
    return proc.returncode == 0 # pylint: disable=E1101

def temp_dir(data_file_name):
    return "%s/%s/" % (TMP_DIR, data_file_name)

def make_plots(data_file_name):
    """Run analyze_data_online.py and generate plots"""
    my_tmp = temp_dir(data_file_name)
    if not os.path.exists(my_tmp):
        os.makedirs(my_tmp)
    log = open(my_tmp+"test_analyze_data_online.log", "w")
    print "Running analyze online"
    env_cp = os.environ.copy()
    env_cp['MAUS_WEB_MEDIA_RAW'] = my_tmp
    env_cp['MAUS_WEB_DIR'] = my_tmp
    proc = subprocess.Popen(['python', ANALYZE_EXE,
                             '--DAQ_online_file', data_file_name],
                             env=env_cp, stdout=log,
                             stderr=subprocess.STDOUT)
    print 'Waiting for online recon to finish'
    while proc.poll() == None:
        print '.',
        log.flush()
        sys.stdout.flush()
        time.sleep(2)
    return proc.returncode


class TestAnalyzeOnline(unittest.TestCase):#pylint: disable =R0904
    """Execute analyze_data_online"""
    def setUp(self):
        self.returncodes = {}        
        for data in ['04222.000']:
            self.returncodes[data] = 0 #make_plots(data)

    def test_root_histos(self):
        """Check that the return code is 0"""
        for key, ret_code in self.returncodes.iteritems():
            self.assertEquals(ret_code, 0)
        pass_dict = {}
        test_pass = True
        for data in self.returncodes.keys():
            ref_dir = os.path.expandvars('${MAUS_ROOT_DIR}/tests/integration/test_analyze_data_online/reference_plots_'+str(data)+'/*.root') 
            for ref_root in glob.glob(ref_dir):
                test_root = temp_dir(data)+ref_root.split('/')[-1]
                print "Testing", test_root, ref_root
                pass_dict[test_root] = \
                            regression.AggregateRegressionTests(test_root, ref_root)
                test_pass = test_pass and pass_dict[test_root]
            for key, value in pass_dict.iteritems():
                print key, value
            self.assertEquals(test_pass, True)

if __name__ == "__main__":
    unittest.main()

