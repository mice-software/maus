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
    print 'Reference dataset generated using',os.environ['MAUS_ROOT_DIR']
    maus_version = open(os.path.expandvars('$MAUS_ROOT_DIR/README')).readline()
    maus_version = maus_version.rstrip('\n')
    maus_version = maus_version.replace(' ', '_')
    test_dir = os.path.expandvars(
             '$MAUS_ROOT_DIR/tests/integration/test_simulation/'+\
             'test_physics_model_full')
    gen_name = os.path.join(test_dir,
             'test_physics_model_tools/generate_reference_data.py')
    out_name = os.path.join(test_dir, 'test_physics_model_ref_data',
             'maus.'+maus_version+'.ref_data.dat')
    test = generate_reference_data.TestFactory('maus')
    test.build_reference_data(generate_reference_data.MICE_CONFIGURATIONS)
    os.rename('maus_ref_data.dat', out_name)

    os.system('rm MaterialBlock.dat')
    os.system('rm maus_output.root')
    os.system('rm configuration.py')

if __name__ == "__main__":
    main()

