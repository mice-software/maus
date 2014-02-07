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
High level tests for the beam maker - some pushed back from unit tests because
they are too slow. Check that we can generate appropriate distributions and that
the simulate_mice.py executable interfaces correctly.

Use Kolmogorov Smirnov test to compare distributions a lot of the time...
"""

import unittest
import subprocess
import os

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "simulate_mice.py")
TEST_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                                "test_simulation", "test_stepping")

def run_simulation(has_rf, stepping_algorithm):
    """
    Run simulation to generate some data. We only want to do this once, so I
    pull it out into a separate part of the test.
    """
    print "Check we can run with RF:", has_rf, "stepping", stepping_algorithm
    config = os.path.join(TEST_DIR, 'no_rf_config.dat')
    out_name = os.path.join(MAUS_ROOT_DIR, "tmp", "test_stepping_algorithm")
    out_name += "_"+stepping_algorithm+"_"+str(has_rf)
    log_file = open(out_name, "w")
    if has_rf:
        config = os.path.join(TEST_DIR, 'rf_config.dat')
    proc = subprocess.Popen([SIM_PATH,
      "--simulation_geometry_filename", config, 
      "--stepping_algorithm", str(stepping_algorithm),
      "--spill_generator_number_of_spills", str(1)],
      stdout=log_file, stderr=subprocess.STDOUT)
    proc.wait() # pylint: disable = E1101
    return proc.returncode # pylint: disable = E1101

class SteppingAlgorithmTest(unittest.TestCase): # pylint: disable = R0904
    """
    Test that stepping datacards work correctly
    """

    def test_stepping_algorithm(self):
        """
        Check that we can set the stepping algorithm okay in Geant4
        """
        rf_tests = ["ClassicalRK4", "Classic", "SimpleHeum", "ImplicitEuler",
                    "SimpleRunge", "ExplicitEuler", "CashKarpRKF45", "SpinTrack"]
        self.assertNotEqual(run_simulation(True, "Nonsense"), 0)
        self.assertNotEqual(run_simulation(False, "Nonsense"), 0)

        for test in rf_tests:
            self.assertEqual(run_simulation(True, test), 0)
            self.assertEqual(run_simulation(False, test), 0)

if __name__ == "__main__":
    unittest.main()

