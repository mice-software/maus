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
Generates plots using test data in ref_data and tmp folder
"""

import glob

from physics_model_test import plotter
from physics_model_test import geometry

def main():
    """
    Search for test data in geometry.ref_data('./') and geometry.temp('./'); if
    found, plot it
    """
    reference_data = glob.glob(geometry.ref_data("*ref_data.dat"))
    test_data = glob.glob(geometry.ref_data("*test_data.dat"))
    plotter.plot(reference_data+test_data)

if __name__ == "__main__":
    main()
    raw_input()
