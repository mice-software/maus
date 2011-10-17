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
import sys
import io

from MapCppSimulation import MapCppSimulation
from Configuration import Configuration

TEST_DIR = '/tests/integration/test_simulation/test_geometry/test_geometry.py'

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

        files = [filename.split('/')[-1] for filename in files]
        print files

        for my_file in files:
            args = ['python', os.environ.get("MAUS_ROOT_DIR")+TEST_DIR, my_file]
            proc = subprocess.Popen(args)
            proc.wait()
            self.assertEqual(proc.returncode, 0)

def my_main():
    """
    Either: if called without arguments, run the tests
            if called with arguments, read a file and exit with -1 if the file
            could not be read
    (Rogers - feels a bit hacky)
    """
    if len(sys.argv) == 1:
        unittest.main()
    if len(sys.argv) == 2:
        my_file = str(sys.argv[1])
        
        my_map = MapCppSimulation()
        
        config_file = io.StringIO(u"simulation_geometry_filename = '%s'\n" \
                                 % my_file)
        conf = Configuration()
        success = my_map.birth(conf.getConfigJSON(config_file))

        if not success:
            sys.exit(-1)
        
        my_map.death()        

if __name__ == '__main__':
    my_main()
