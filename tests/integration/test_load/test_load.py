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
import argparse

import process_monitor #pylint: disable = F0401

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


def get_sim_proc(valgrind):
    """Start up a simulate_mice process"""
    sim_path = MRD+"/bin/simulate_mice.py"  
    config = TEST_DIR+"/test_load_configuration"
    sim_options = [sim_path, "--configuration_file", config]
    sim_log = open(TMP_DIR+"/test_load_simulate_mice.log", "w")
    if valgrind:
        sim_options = ['valgrind', '--leak-check=yes', 'python']+sim_options
    proc = subprocess.Popen(sim_options, stdout=sim_log,
                                         stderr=subprocess.STDOUT)
    return proc

def get_rec_proc(run_number, valgrind):
    """Start up a analyze_data_offline process"""
    rec_path = MRD+"/bin/analyze_data_offline.py"
    config = TEST_DIR+"/test_load_configuration"
    rec_options = [rec_path, "--configuration_file", config,
                   "--daq_data_file", run_number]
    if valgrind:
        rec_options = ['valgrind', '--leak-check=yes', 'python']+rec_options
    rec_log = open(TMP_DIR+"/test_load_analyze_data_offline_"+run_number+".log",
                   "w")
    proc = subprocess.Popen(rec_options, stdout=rec_log,
                                         stderr=subprocess.STDOUT)
    return proc

def arg_parser():
    """
    Parse command line arguments.

    Use -h switch at the command line for information on command line args used.
    """
    parser = argparse.ArgumentParser(description="Runs MAUS reconstruction "+\
                          "and checks that memory usage is below a threshold.")
    parser.add_argument('--run-number', dest='run_number',
                        help='Run number to test against. '+\
                        'Set to 0 to run against a default scenario, '+\
                        'including a bit of MC',
                        default=0, type=int)
    parser.add_argument('--max-mem-size', dest='max_mem_size',
                        help='Fail if more memory is used by a process',
                        default=400000, type=int)
    parser.add_argument('--max-time', dest='max_time',
                        help='Kill the process if it takes more time [s]',
                        default=600, type=int)
    parser.add_argument('--time-step', dest='time_step',
                        help='Time step for checking the process memory size',
                        default=5, type=int)
    parser.add_argument('--valgrind', dest='valgrind',
                        help='Set to 1 to run valgrind leak-check=yes',
                        default=0, type=int)
    return parser

class LoadTest(unittest.TestCase): # pylint: disable = R0904
    """
    Look for memory usage over a long duration MC
    """
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """
        Class setup
        """
        if not cls.args:
            cls.args = arg_parser().parse_args([]) # defaults

    def check_resource_usage(self,
                             resource_usage_list,
                             max_mem_size):
        """
        Check that resource usage does not grow excessively

        We want to check that the resource usage for MAUS does not get out of
        hand
        - resource_usage_list output from the process_monitor; list looks like
          [
            [
                {resource usage for process A at time step},
                {resource usage for process B at time step},
                ...
            ]
          ]
          so the first array indexes the time, the second indexes the process
        - max_mem_size - fail the test if mem size is ever greater than max
        """
        for resource_usage in resource_usage_list:
            for proc_usage in resource_usage:
                if "sz" in proc_usage and \
                   float(proc_usage["sz"]) > max_mem_size:
                    print "Failed", proc_usage
                    self.assertLess(float(proc_usage["sz"]),
                                    max_mem_size)

    def test_simulate_mice(self):
        """
        Test that we can simulate mice for a long duration without memory 
        problems
        """
        do_valgrind = self.args.valgrind == 1
        if self.args.run_number == 0:
            self.run_number = self.default_run_number
            get_data(self.run_number)
            procs = [get_sim_proc(do_valgrind), get_rec_proc(self.run_number,
                                                             do_valgrind)]
            title = PLOT_DIR+"maus"
        else:
            self.run_number = str(self.args.run_number).rjust(5, "0")
            get_data(self.run_number)
            procs = [get_rec_proc(self.run_number, do_valgrind)]
            title = PLOT_DIR+"run_"+str(self.run_number)
        try:
            resource_usage_list = process_monitor.main(
                                [proc.pid for proc in procs],
                                title,
                                self.args.time_step,
                                self.args.max_time)
            for proc in procs:
                self.assertFalse(proc.returncode) # no crash during running
            self.check_resource_usage(resource_usage_list,
                                      self.args.max_mem_size)
        except Exception:
            raise
        finally:
            for proc in procs:
                print "Killing process", proc.pid
                proc.send_signal(signal.SIGINT)
            time.sleep(max(5, self.args.max_time/100))
            for proc in procs:
                while proc.poll() == None:
                    proc.send_signal(signal.SIGKILL)
                    time.sleep(5)
                print "killed with return code", proc.returncode

    args = None
    run_number = None
    default_run_number = "04258"

if __name__ == "__main__":
    LoadTest.args = arg_parser().parse_args(sys.argv[1:])
    sys.argv = [sys.argv[0]]
    unittest.main()
