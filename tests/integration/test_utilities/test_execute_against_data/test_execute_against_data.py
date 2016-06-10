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
Test execute_against_data (batch script for the GRID)
"""

import unittest
import os
import sys
import shutil
import glob
import urllib
import subprocess
import tarfile
import time
import ROOT
import cdb
import libMausCpp # pylint: disable=W0611

RUN_NUMBER = "03541"
TEST_URL = "http://www.hep.ph.ic.ac.uk/micedata/MICE/Step1/03500/"
TEST_FILE = RUN_NUMBER+".tar"
TEST_OUT  = RUN_NUMBER+"_offline.tar"
TEST_DIR = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp",
                        "test_execute_against_data")
TEST_FILE_EXISTS = False
UNPACK_STEP4 = False

# set a different test file for stepIV
if os.environ['MAUS_UNPACKER_VERSION'] == "StepIV":
    RUN_NUMBER = "06008"
    TEST_URL = "http://www.hep.ph.ic.ac.uk/micedata/MICE/Step4/06000/"
    TEST_FILE = RUN_NUMBER+".tar"
    TEST_OUT  = RUN_NUMBER+"_offline.tar"
    UNPACK_STEP4 = True

def get_data():
    """
    Clean up test directory. If the tarball already exists, preserve it - else
    pull down a new copy
    """
    global TEST_FILE_EXISTS # pylint: disable = W0603
    test_target = os.path.join(TEST_DIR, TEST_FILE)
    print 'Cleaning test directory'
    if not os.path.isdir(TEST_DIR):
        os.mkdir(TEST_DIR)
    for item in glob.glob(TEST_DIR+"/*"):
        if os.path.isdir(item):
            shutil.rmtree(item)
        elif os.path.isfile(item) and item != test_target:
            os.remove(item)
    if not os.path.isfile(test_target):
        print 'Getting ', TEST_URL+TEST_FILE,
        try:
            urllib.urlretrieve(TEST_URL+TEST_FILE, test_target, url_hook)   
            TEST_FILE_EXISTS = True
            print ' ... Finished'
        except IOError:
            print "Couldn't get file"
            sys.excepthook(*sys.exc_info())
            TEST_FILE_EXISTS = False
    else:
        print 'Found', TEST_FILE, 'so not going to download'
        TEST_FILE_EXISTS = True
    return TEST_FILE_EXISTS

OLD_TIME = -20
TIME = 0
def url_hook(transferred, size, total):
    """
    Hook for monitoring download
    """
    global TIME, OLD_TIME # pylint: disable = W0603
    TIME = time.clock()
    if TIME-OLD_TIME > 1:
        print
        print 'Downloaded', round(transferred*size/1048576., 1), 'MB out of', \
                                                 round(total/1048576., 1), 'MB',
        OLD_TIME = TIME

def clean_dir():
    """
    Clean all the gumph out
    """
    for item in glob.glob(TEST_DIR+"/*"):
        if os.path.isdir(item):
            print 'removing dir', item
            shutil.rmtree(item)
        if os.path.isfile(item) and item[-3:] != 'tar':
            print 'removing file', item
            os.remove(item)

class TestMain(unittest.TestCase): # pylint: disable = R0904
    """
    Test the main method of the batch script
    """
    def __check_sim_file(self):
        """
        Make some simple data integrity checks on the simulation file
        """
        root_file = ROOT.TFile(RUN_NUMBER+'_sim.root', # pylint: disable = E1101
                                               "READ")
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)
        self.assertGreater(tree.GetEntries(), 0)
        tree.GetEntry(0)
        self.assertGreater(data.GetSpill().GetMCEventSize(), 1)
        root_file.Close()

    def __check_recon_file(self):
        """
        Make some simple data integrity checks on the simulation file
        """
        root_file = ROOT.TFile(RUN_NUMBER+'_recon.root', # pylint: disable=E1101
                                               "READ")
        data = ROOT.MAUS.Data() # pylint: disable = E1101
        tree = root_file.Get("Spill")
        tree.SetBranchAddress("data", data)
        self.assertGreater(tree.GetEntries(), 0)
        tree.GetEntry(3)
        self.assertEqual(data.GetSpill().GetDaqEventType(), "physics_event")
        self.assertGreater(data.GetSpill().GetReconEventSize(), 0)
        # not yet ready to test detector hits in the step4 test file
        # this will have to wait for meaningful data to come out of step4
        if not UNPACK_STEP4:
            self.assertGreater(data.GetSpill().GetAReconEvent(0).GetTOFEvent().\
                                  GetTOFEventDigit().GetTOF1DigitArraySize(), 0)
        root_file.Close()


    # want to test also each of the return codes is okay
    def test_main_success(self):
        """
        Test that the GRID batch script works

        Test script runs, the correct files are put in the tarball and the
        simulation and reconstruction have some data in them
        """
        bi_super = cdb.BatchIterationSuperMouse(
                                              "http://preprodcdb.mice.rl.ac.uk")
        bi_index = 0
        for bi_index in range(50000):
            if bi_super.get_mc_datacards(bi_index)['mc'] == 'null':
                break # the bi number does not exist - we can set it
        print 'Found free row with batch iteration number', bi_index
        bi_super.set_datacards(bi_index, "test data", "", "")

        if not get_data():
            # raise RuntimeError('Error downloading file - abort test')
            # sometimes the IC data host goes down - skip if network glitch
            print 'Error downloading file - skipping further tests'
            return
        here = os.getcwd()
        os.chdir(TEST_DIR)
        self.test_args.append(str(bi_index))
        # prod_args tries to call the production server and run with bi index 1
        # (default cards) - disabled for now as default cards are not yet loaded
        for args in [self.test_args]:#, self.prod_args]:
            print "Running arguments\n    "+str(args)
            proc = subprocess.Popen(args)
            proc.wait()
            self.assertEqual(proc.returncode, 0)
            clean_dir()
            print '  Unpacking for testing'
            tar_out = tarfile.open(TEST_OUT, 'r:*')
            tar_out.extractall()
            for file_name in [TEST_OUT, TEST_FILE, 'batch.log', 'download.log',
                       'reco.log', 'sim.log', RUN_NUMBER+'_recon.root',
                       RUN_NUMBER+'_sim.root']:
                self.assertTrue(os.path.isfile(file_name), file_name)
            self.__check_sim_file()
            self.__check_recon_file()

        os.chdir(here)

    test_args = [
        os.path.join(os.environ["MAUS_ROOT_DIR"], "bin", "utilities",
                                                 "execute_against_data.py"),
        "--input-file", TEST_FILE,
        "--test",
        "--batch-iteration"
    ]

    prod_args = [
        os.path.join(os.environ["MAUS_ROOT_DIR"], "bin", "utilities",
                                                 "execute_against_data.py"),
        "--input-file", TEST_FILE,
        "--batch-iteration", "1"]

if __name__ == "__main__":
    unittest.main()


