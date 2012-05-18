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
import json
import time

RUN_NUMBER = "03541"
TEST_URL = "http://www.hep.ph.ic.ac.uk/micedata/MICE/Step1/03500/"
TEST_FILE = RUN_NUMBER+".tar"
TEST_OUT  = RUN_NUMBER+"_offline.tar"
TEST_DIR = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", "test_batch")
TEST_FILE_EXISTS = False

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
    # want to test also each of the return codes is okay
    def test_main_success(self):
        """
        Test that the GRID batch script works

        Test script runs, the correct files are put in the tarball and the
        simulation and reconstruction have some data in them
        """
        if not get_data():
            print 'Error downloading file - abort test'
            return
        here = os.getcwd()
        os.chdir(TEST_DIR)
        args = [
            os.path.join(os.environ["MAUS_ROOT_DIR"], "bin", "utilities",
                                                     "execute_against_data.py"),
            "--input-file", TEST_FILE,
            "--test",]
        proc = subprocess.Popen(args)
        proc.wait()
        self.assertEqual(proc.returncode, 0)
        clean_dir()
        print 'Unpacking for testing'
        tar_out = tarfile.open(TEST_OUT, 'r:*')
        tar_out.extractall()
        for file_name in [TEST_OUT, TEST_FILE, 'batch.log', 'download.log', \
                   'reco.log', 'sim.log', RUN_NUMBER+'_recon.json', \
                   RUN_NUMBER+'_sim.json']:
            self.assertTrue(os.path.isfile(file_name), file_name)
        sim_first_line = open(RUN_NUMBER+'_sim.json').readline()
        sim_first_spill = json.loads(sim_first_line)
        self.assertGreater(len(sim_first_spill['mc']), 1)
        rec_in = open(RUN_NUMBER+'_recon.json')
        # start of run twice (?), start of burst 
        for i in range(3): # pylint: disable = W0612
            rec_in.readline()
        rec_first_spill = json.loads(rec_in.readline())
        self.assertTrue('mc' not in rec_first_spill.keys())
        self.assertGreater(len(rec_first_spill['digits']['tof1']), 0)
        os.chdir(here)

if __name__ == "__main__":
    unittest.main()


