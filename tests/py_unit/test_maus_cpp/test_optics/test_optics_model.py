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

# pylint: disable=C0103

"""
Test maus_cpp.optics_model
"""

import os
import subprocess
import unittest

class OpticsModelTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.optics_model"""
    def test_optics_model(self):
        """
        Test optics model runs in a subprocess to prevent pollution of globals
        """
        test = os.path.expandvars(\
              "${MAUS_ROOT_DIR}/tests/py_unit/test_maus_cpp/"+\
              "test_optics/test_optics_model_subprocess")
        subprocess.check_call(test)

if __name__ == "__main__":
    unittest.main()

