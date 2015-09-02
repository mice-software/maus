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

"""Check that the latest geometry runs and generate validation plots"""

import unittest
import shutil
import os 
import subprocess
import time
import sys

import cdb

MONITOR = os.path.expandvars("${MAUS_ROOT_DIR}/tests/integration/test_load")
sys.path.append(MONITOR) 
import process_monitor #pylint: disable=F0401

def validate_geometry(geometry_id = None):
    """Download a geometry and run the geometry_validation script"""
    if geometry_id == None:
        all_ids = cdb.Geometry().get_ids("1970-01-01 00:00:00", 
                                         "2050-01-01 00:00:00").keys()
        geometry_id = sorted(all_ids)[-1]
    print "Validating geometry id", geometry_id
    mrd = os.path.expandvars("${MAUS_ROOT_DIR}")
    mtd = os.path.expandvars("${MAUS_TMP_DIR}")
    test_path =  mrd+"/tests/integration/test_utilities/test_geometry/"
    download = mrd+"/bin/utilities/download_geometry.py"
    validate = mrd+"/bin/utilities/geometry_validation.py"
    path = mtd+"/test_latest_geometry_"+str(geometry_id)
    shutil.rmtree(path, True)
    os.makedirs(path+"/geometry")
    fout = open(path+"/test_latest_geometry.log", "w")
    os.chdir(path)
    proc = subprocess.Popen(["python", download,
                             "--geometry_download_by", "id",
                             "--geometry_download_directory", \
                             path + "/geometry", \
                             "--geometry_download_id", str(geometry_id)],
                            stdin=subprocess.PIPE)
    while proc.poll() == None:
        proc.communicate('\n')
        time.sleep(10)
    proc.wait()
    testpass = proc.returncode == 0
    for conf in ["conf1", "conf2"]:
        max_mem = 0
        print >> fout, "# Running configuration", conf
        proc = subprocess.Popen(["python", validate,
                                 "--configuration_file", test_path+conf],
                                 stdin=subprocess.PIPE)
        while proc.poll() == None:
            mem_usage = process_monitor.print_mem_usage(proc.pid)
            print >> fout, process_monitor.print_mem_usage(proc.pid)
            if int(mem_usage['rsz']) > 2.5e6:
                testpass = False
            if int(mem_usage['rsz']) > max_mem:
                max_mem = int(mem_usage['rsz'])
            fout.flush()
            proc.stdin.write('\n')
            fout.flush()
            time.sleep(10)
        proc.communicate('\n')
        print "Maximum memory usage is ",max_mem
        testpass &= proc.returncode == 0
    return testpass

class TestLatestGeometry(unittest.TestCase): #pylint: disable=R0904
    """Test latest geometry downloads, builds and runs"""
    def test_geometry(self):
        """Test latest geometry downloads, builds and runs"""
        self.assertTrue(validate_geometry())

if __name__ == "__main__":
    unittest.main()

