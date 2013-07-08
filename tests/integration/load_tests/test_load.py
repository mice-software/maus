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
Check for memory leaks on running over a large number of events

Monitor memory usage over several hours of MC
"""

import unittest
import os
import time
import subprocess
import signal

import process_monitor

MAX_RUN_TIME = 60*10*1 # 8 hours

class SimulationLoadTest(unittest.TestCase): # pylint: disable = R0904
    """
    Look for memory usage over a long duration MC
    """

    def test_simulate_mice(self):
        """
        Test that we can simulate mice for a long duration without memory 
        problems
        """
        sim_path = os.path.expandvars("${MAUS_ROOT_DIR}/bin/simulate_mice.py")
        sim_options = [sim_path, "--configuration_file",
                                      "load_test_configuration.py"]
        sim_log = open("test_load_simulate_mice.log", "w")
        proc = subprocess.Popen(sim_options, stdout=sim_log,
                                             stderr=subprocess.STDOUT)
        try:
            process_monitor.main([proc.pid], 5, MAX_RUN_TIME)
            self.assertEqual(proc.poll(), None) # no crash during running
        except Exception:
            raise
        finally:
            print "killing process", proc.pid
            proc.send_signal(signal.SIGINT)
            time.sleep(5)
            while proc.poll() == None:
                proc.send_signal(signal.SIGKILL)
                time.sleep(5)
            print "killed with return code", proc.returncode

if __name__ == "__main__":
    unittest.main()

