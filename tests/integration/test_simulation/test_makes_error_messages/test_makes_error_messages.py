"""
Test that we make appropriate error messages
"""

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

import subprocess
import os
import unittest

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
TMP_PATH = os.path.join(MAUS_ROOT_DIR, "tmp")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "simulate_mice.py")
TEST_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", 
                                "test_simulation", "test_makes_error_messages")


class TestMakesErrorMessages(unittest.TestCase): #pylint: disable = R0904
    """
    Check that we generate some useful error messages when running - test for
    issue #799
    """

    def test_makes_error_messages(self):
        """
        Check that we generate some useful error messages when running
        simulation

        Test for issue #799
        """
        log_file_name = os.path.join(TMP_PATH, 'test_makes_error_messages.log')
        out = open(log_file_name, 'w')
        subproc = subprocess.Popen([SIM_PATH, '-configuration_file', \
                               os.path.join(TEST_DIR, 'birth_error.py')],
                               stdout = out, stderr = out)
        subproc.wait()
        subproc = subprocess.Popen([SIM_PATH, '-configuration_file', \
                               os.path.join(TEST_DIR, 'process_error.py'),
                               '-simulation_geometry_filename', 
                               os.path.join(TEST_DIR, 'Test.dat')],
                               stdout = out, stderr = out)
        subproc.wait()
        out.close()

        found_birth_error, found_process_error = False, False
        fin = open(log_file_name)
        for line in fin.readlines():
            if line.find( 'Failed to open root module file at') > -1:
                found_birth_error = True
            if line.find( 'Particle pid not recognised at') > -1:
                found_process_error = True
        self.assertTrue(found_birth_error)
        self.assertTrue(found_process_error)

if __name__ == "__main__":
    unittest.main()


