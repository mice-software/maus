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
Support script to generate reference dataset for MAUS and put it in the 
appropriate place
"""

import os
import subprocess
import generate_reference_data

def main():
    """
    Generate a reference dataset for MAUS and put it in
    test_physics_model_ref_data
    """
    print 'Reference dataset generated using', os.environ['MAUS_ROOT_DIR'],
    test = generate_reference_data.TestFactory(generate_reference_data.MausConvert())
    test.build_reference_data(generate_reference_data.MICE_CONFIGURATIONS)

if __name__ == "__main__":
    main()

