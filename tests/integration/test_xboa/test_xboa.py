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
Run xboa tests as part of MAUS integration tests
"""

# pylint: disable=E1101

import unittest
import subprocess
import os
import glob

class TestXBOA(unittest.TestCase): # pylint: disable=R0904
    """
    Run the tests and check they return 0
    """
    def setUp(self): # pylint: disable = C0103
        """Set up; skip if no xboa installs found"""
        self.xboa_test = None
        self._get_test_target()
        if self.xboa_test == None:
            raise unittest.SkipTest("No xboa installed in third party")

    def _get_test_target(self):
        """
        Look for xboa versions in third_party/build of *this* MAUS install;
        if none found, return, else set self.xboa_test to the most recent
        version (as determined by folder name)
        """
        xboa_path = os.path.expandvars(
                                    "${MAUS_ROOT_DIR}/third_party/build/*xboa*")
        xboa_installs = glob.glob(xboa_path)
        if len(xboa_installs) == 0:
            # we only run tests if xboa is installed in this setup (rather than
            # installed from somewhere else)
            return
        else:
            # this loop is supposed to test on the most recent version of xboa
            # installed in case of multiple versions; or a dev version if it
            # is available
            xboa_installs = sorted(xboa_installs)
            for self.xboa_test in xboa_installs:
                if "dev" in self.xboa_test:
                    break

    def test_xboa(self):
        """
        Run the xboa test
        """
        os.chdir(self.xboa_test+"/test")
        subprocess.check_output(["python", "XBOATest.py"])


if __name__ == "__main__":
    unittest.main()

