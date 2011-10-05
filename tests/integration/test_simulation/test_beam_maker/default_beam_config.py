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
Configuration to generate a beam distribution with default values - just to
do sanity check that we generate a beam
"""

#pylint: disable = C0103, R0801

import os
mrd = os.environ["MAUS_ROOT_DIR"]
simulation_geometry_filename = os.path.join(
  mrd, "tests", "integration", "test_simulation", "test_beam_maker", 
  "BeamTest.dat"
)

