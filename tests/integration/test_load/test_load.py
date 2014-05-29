#  This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
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
Check for memory leaks on running over a large number of events

Monitor memory usage over several hours of MC
"""

import unittest
import os
import time
import subprocess
import signal
import sys

import process_monitor #pylint: disable = F0401

MAX_RUN_TIME = 60*10*1 # 10 minutes
PLOT_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/tests/integration/plots/")
TMP_DIR = os.path.expandvars("${MAUS_TMP_DIR}")
MRD = os.path.expandvars("${MAUS_ROOT_DIR}")
MTP = os.path.expandvars("${MAUS_THIRD_PARTY}")
TEST_DIR = os.path.expandvars("${MAUS_ROOT_DIR}/tests/integration/test_load/")

def get_data(run_number):
    """Get data for run number"""
    proc = subprocess.Popen(["bash",
                             MTP+"/third_party/bash/71test_data.bash",
                             str(run_number)])
    proc.wait()


def get_sim_proc():
    """Start up a simulate_mice process"""
    sim_path = MRD+"/bin/simulate_mice.py"  
    config = TEST_DIR+"/test_load_configuration"
    sim_options = [sim_path, "--configuration_file", config]
    sim_log = open(TMP_DIR+"/test_load_simulate_mice.log", "w")
    proc = subprocess.Popen(sim_options, stdout=sim_log,
                                         stderr=subprocess.STDOUT)
    return proc

def get_rec_proc(run_number):
    """Start up a analyze_data_offline process"""
    rec_path = MRD+"/bin/analyze_data_offline.py"
    config = TEST_DIR+"/test_load_configuration"
    rec_options = [rec_path, "--configuration_file", config,
                   "--daq_data_file", run_number]
    rec_log = open(TMP_DIR+"/test_load_analyze_data_offline.log", "w")
    proc = subprocess.Popen(rec_options, stdout=rec_log,
                                         stderr=subprocess.STDOUT)
    return proc


class LoadTest(unittest.TestCase): # pylint: disable = R0904
    """
    Look for memory usage over a long duration MC
    """
    def check_resource_usage(self,
                             resource_usage_list,
                             fraction_of_time,
                             mem_threshold_factor):
        """
        Check that resource usage does not grow excessively
        """
        pass

    def test_simulate_mice(self):
        """
        Test that we can simulate mice for a long duration without memory 
        problems
        """
        sim_proc = get_sim_proc()
        rec_proc = get_rec_proc(self.run_number)
        try:
            resource_usage_list = process_monitor.main(
                                [sim_proc.pid, rec_proc.pid], 
                                PLOT_DIR+"maus",
                                5,
                                MAX_RUN_TIME)
            self.assertFalse(sim_proc.returncode) # no crash during running
            self.assertFalse(rec_proc.returncode) # no crash during running
            self.check_resource_usage(resource_usage_list, 0.5, 2.)
        except Exception:
            raise
        finally:
            for proc in sim_proc, rec_proc:
                print "Killing process", proc.pid
                proc.send_signal(signal.SIGINT)
            time.sleep(5)
            for proc in sim_proc, rec_proc:
                while proc.poll() == None:
                    proc.send_signal(signal.SIGKILL)
                    time.sleep(5)
            print "killed with return code", proc.returncode

    run_number = "04258"

if __name__ == "__main__":
    if len(sys.argv) > 1:
        LoadTest.run_number = sys.argv[1]
        sys.argv = [sys.argv[0]]
        get_data(LoadTest.run_number)
    unittest.main()

