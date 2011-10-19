# This file is part of MAUS: http://micewww.pp.rl.ac.uk/projects/maus
#
# MAUS is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# MAUS is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#

"""
Check that we can load the default geometries okay
"""

import unittest
import subprocess
import glob
import os

class MapCppSimulationTestCase(unittest.TestCase): # pylint: disable = R0904
    """
    Test that we can load default geometries okay
    """
    @classmethod
    def setUpClass(self): # pylint: disable = C0103, C0202
        """
        Check that MICEFILES environment variable is set
        """
        if not os.environ.get("MICEFILES"):
            raise Exception('InitializeFail', \
                            'MICEFILES environmental variable not set')

        if not os.path.isdir(os.environ.get("MICEFILES")):
            raise Exception('InitializeFail', 'MICEFILES is not a directory')

    def __run_sim(self, filename):
        """
        Run the simulation with configuration file given by filename

        \param filename simulation_geometry_filename
        """
        maus_root_dir = os.environ.get("MAUS_ROOT_DIR")
        test_dir = maus_root_dir+'/bin/simulate_mice.py'
        config_dir = maus_root_dir+\
                    '/tests/integration/test_simulation/test_geometry/'
        out_dir = maus_root_dir+'/tmp/'

        args = [test_dir, \
                '-simulation_geometry_filename', filename, \
                '-configuration_file', config_dir+'config.py']
        outname = out_dir+'/test_geometry_'+filename
        my_stdout = open(outname, 'w')
        proc = subprocess.Popen(args, stdout=my_stdout)
        proc.wait()
        my_stdout.close()
        self.assertEqual(proc.returncode, 0, 
            msg="Test geometry failed in "+filename+" with return code "+\
                str(proc.returncode)+". Output in "+outname)

    def test_geometries(self):
        """
        For each "Configuration" file, check that we can load it - by calling
        this test with an argument. If yes, then load that file
        """
        self.assertTrue(os.environ.get("MICEFILES") != None)
        self.assertTrue(os.path.isdir(os.environ.get("MICEFILES")))

        self.assertTrue(os.environ.get("MAUS_ROOT_DIR") != None)
        self.assertTrue(os.path.isdir(os.environ.get("MAUS_ROOT_DIR")))
        
        files = glob.glob("%s/Models/Configurations/*.dat" % \
                          os.environ.get("MICEFILES"))
        self.assertTrue(len(files) != 0)

        files = sorted([filename.split('/')[-1] for filename in files])

        for my_file in files:
            self.__run_sim(my_file)

def my_main():
    """
    Attempt to run every geometry with a toy beam
    """
    unittest.main()

if __name__ == '__main__':
    my_main()
