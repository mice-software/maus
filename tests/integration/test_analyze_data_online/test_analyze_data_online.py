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

# pylint: disable=E1101

"""
test_analyze_online.py - Note that I never ran this test successfully! Always
skips until someone sorts out the environment on the test server.
"""

import shutil
import sys
import os
import subprocess
import signal
import unittest
import time
import glob

import regression
import ROOT
ROOT.gROOT.SetBatch(True)

ONLINE_TEST = os.path.expandvars(
                            "$MAUS_ROOT_DIR/tests/integration/"+\
                            "test_distributed_processing/_test_online_okay.py")
ANALYZE_EXE = os.path.expandvars("$MAUS_ROOT_DIR/bin/analyze_data_online.py")
TMP_DIR =  os.path.expandvars("$MAUS_ROOT_DIR/tmp/test_analyze_data_online/")
LOCKFILE = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp', '.maus_lockfile')

def temp_dir(data_file_name):
    """
    Return the directory name for temp data associated with given file
    """
    return "%s/%s/" % (TMP_DIR, data_file_name)

def run_process(data_file_name, dir_suffix, send_signal=None):
    """Run analyze_data_online.py and generate plots"""
    if os.path.exists(LOCKFILE):
        raise RuntimeError("Found lockfile")
    my_tmp = temp_dir(data_file_name+dir_suffix)
    if not os.path.exists(my_tmp):
        os.makedirs(my_tmp)
    raw_dir = my_tmp+'/raw'
    print raw_dir, os.path.exists(raw_dir), my_tmp, os.path.exists(my_tmp)
    if os.path.exists(raw_dir):
        os.remove(raw_dir)
        time.sleep(1)
    os.symlink(my_tmp, raw_dir)
    log = open(my_tmp+"test_analyze_data_online.log", "w")
    print "Running analyze online"
    print "Point your browser at http://localhost:9000/maus/"
    env_cp = os.environ.copy()
    env_cp['MAUS_WEB_MEDIA'] = my_tmp+'/'
    env_cp['MAUS_WEB_MEDIA_RAW'] = my_tmp+'/raw/'
    proc = subprocess.Popen(['python', ANALYZE_EXE,
                             '--DAQ_online_file', TMP_DIR+data_file_name],
                             env=env_cp, stdout=log,
                             stderr=subprocess.STDOUT)
    if send_signal != None:
        time.sleep(5)
        proc.send_signal(send_signal)
    while proc.poll() == None:
        time.sleep(5)
        print '.',
        sys.stdout.flush()
    print '\n'
    return proc.returncode

class TestAnalyzeOnline(unittest.TestCase):#pylint: disable =R0904
    """Execute analyze_data_online"""
    def setUp(self): # pylint: disable=C0103
        """
        Clear any lockfile that exists
        """
        if os.path.exists(LOCKFILE):
            os.remove(LOCKFILE)
            print 'Cleared lockfile'
            time.sleep(1)
        if os.path.exists(TMP_DIR):
            shutil.rmtree(TMP_DIR)
        os.makedirs(TMP_DIR)

        target =  TMP_DIR+"test_data.cat"
        if os.path.lexists(target):
            print 'Removing', target
            os.remove(target)
            time.sleep(1)
        share = os.path.expandvars(
                     "${MAUS_THIRD_PARTY}/third_party/install/share/test_data/")
        share = share+"test_data.cat"
        print "Linking", share, "to", target 
        os.symlink(share, target)
        online_okay = os.path.expandvars('$MAUS_ROOT_DIR/tests/integration/'+\
                            'test_distributed_processing/_test_online_okay.py')
        proc = subprocess.Popen(['python', online_okay],
                                stdout=open(TMP_DIR+'online_okay.log', 'w'),
                                stderr=subprocess.STDOUT)
        proc.wait()

        if proc.poll() != 0:
            unittest.TestCase.skipTest(self, "Skip - online is not available")

    def _test_kill(self):
        """
        Check that analyze_data_online dies on sigkill with non-zero return code
        """
        returncode = run_process('04235.000', '_sigkill', signal.SIGKILL)
        self.assertNotEqual(returncode, 0)

    def _test_keyboard_interrupt(self):
        """
        Check that analyze_data_online dies on sigint with zero return code
        """
        returncode = run_process('04235.000', '_sigint', signal.SIGINT)
        self.assertEquals(returncode, 0)

    def test_root_histos(self):
        """
        Check that analyze_data_online makes good histos for full run
        """
        # test_data.cat is a merge of several runs; we look for histos from the
        # last run and check that they are correct; note sometimes we get false
        # fails because online recon is still processing the previous run during
        # next run processing (because we don't have appropriate time delay
        # between spills)
        self.assertEquals(0, run_process("test_data.cat", '_histos'))
        pass_dict = {}
        test_pass = True
        # ROOT Chi2 is giving False negatives (test fails) so we exclude 
        test_config = [regression.KolmogorovTest(0.1, 0.05)]
        for data in ['test_data.cat_histos']:
            ref_dir = os.path.expandvars('${MAUS_ROOT_DIR}/tests/integration'+\
               '/test_analyze_data_online/reference_plots_04235.000/')
            for ref_root in glob.glob(ref_dir+'*.root'):
                test_root = temp_dir(data)+ref_root.split('/')[-1]
                pass_dict[test_root] = regression.AggregateRegressionTests(
                                                   test_root,
                                                   ref_root,
                                                   default_config = test_config)
                test_pass = test_pass and pass_dict[test_root]
            for key, value in pass_dict.iteritems():
                print 'test file:', key, 'passes:', value
            self.assertEquals(test_pass, True)
        test_dir = os.path.expandvars('$MAUS_ROOT_DIR/tmp/'+\
                             'test_analyze_data_online/test_data.cat_histos/')
        eor_dir = test_dir+'end_of_run/4235/'
        self.assertTrue(os.path.exists(eor_dir), msg="Failed to find "+eor_dir)
        ref_png = [item.split('/')[-1] for item in glob.glob(test_dir+'*.png')]
        eor_png = [item.split('/')[-1] for item in glob.glob(eor_dir+'*.png')]
        for item in ref_png:
            self.assertTrue(item in eor_png, msg = "Failed to find '"+item+\
                            "' in "+str(eor_dir)+" "+str(eor_png))
        for item in eor_png:
            self.assertTrue(item in ref_png, msg = "Failed to find '"+item+\
                            "' in "+str(eor_dir)+" "+str(ref_png))
      

if __name__ == "__main__":
    unittest.main()

