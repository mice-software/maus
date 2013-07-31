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

import unittest

import numpy

import maus_cpp
from maus_cpp.phase_space_vector import PhaseSpaceVector

class OpticsModelTestCase(unittest.TestCase): # pylint: disable=R0904
    """Test maus_cpp.optics_model"""
    def setUp(self):
        """does nothing"""
        pass

    def test_init(self):
        """Test maus_cpp.optics_model.__init__() and deallocation"""
        psv = PhaseSpaceVector()

if __name__ == "__main__":
    unittest.main()


