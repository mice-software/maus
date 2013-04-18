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
Get tracking speed as a function of n_spills
"""

import time
import unittest
import os
import subprocess
import json

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
TMP_PATH = os.path.join(MAUS_ROOT_DIR, "tmp")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "simulate_mice.py")
TEST_PATH = os.path.join(MAUS_ROOT_DIR,
                        "tests/integration/test_simulation/test_tracking_speed")
 
class TestTrackingSpeed(unittest.TestCase): # pylint: disable=R0904
    """
    Test class to check tracking speed
    """

    def test_tracking_speed(self): # pylint: disable=R0201
        """
        Run simulation and time execution time.
        """
        log = open(TMP_PATH+'/test_tracking_speed.log', 'w')
        out = open(TMP_PATH+'/test_tracking_speed.json', 'w')
        
        # run simulation
        config = os.path.join(TEST_PATH, 'configuration.py')
        results = []
        for n_spills in [0, 1, 5, 10, 50]:
            time_start = time.time()
            subproc = subprocess.Popen([
                           SIM_PATH,
                           '--configuration_file', config,
                           '--spill_generator_number_of_spills', str(n_spills)],
                           stdout = log, stderr=subprocess.STDOUT)
            subproc.wait() # pylint: disable=E1101
            print "Took", time.time() - time_start, "seconds to track", \
                  n_spills, "spills each with 10 muons"
            results += {"dt":time.time()-time_start, "n_spills":n_spills}
        out.write(json.dumps(results))


if __name__ == "__main__":
    unittest.main()

