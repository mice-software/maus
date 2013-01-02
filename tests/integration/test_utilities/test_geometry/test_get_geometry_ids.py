"""
test_get_geometry_ids.py
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

import sys
import unittest
import os
import subprocess
import urllib2
from geometry.ConfigReader import Configreader

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
TMP_PATH = os.path.join(MAUS_ROOT_DIR, "tmp")
SIM_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "utilities", \
                                                          "get_geometry_ids.py")
TEST_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                        "test_utilities", "test_geometry")
CONFIG_PATH_FILE = os.path.join(TEST_DIR, "test_get_ids_config.txt")
CONFIG_PATH_NO_FILE = os.path.join(TEST_DIR, "test_get_ids_config_nofile.txt")

def run_get_geometries():
    """
    Run get_geometry_ids to generate some data. 
    """
    file_name = os.environ['MAUS_ROOT_DIR']+'/tmp/get_geometry_ids_output'
    test_out = open(file_name, 'w')
    subproc = subprocess.Popen([SIM_PATH, \
                           '-configuration_file', CONFIG_PATH_NO_FILE],\
                                      stdout=test_out, stderr=subprocess.STDOUT)
    subproc.wait()
    test_out.close()
    subproc = subprocess.Popen([SIM_PATH, \
                           '-configuration_file', CONFIG_PATH_FILE])
    subproc.wait()

class TestGetGeometryIDS(unittest.TestCase): #pylint:disable= R0904
    """
    This test calls the get_geometry_ids specifying the executable to
    both print to screen and create a file of geometry ids. It captures
    the screen print to file and checks the correct information can
    be found within it. It also checks the created file for the same
    information. It will not carry out the test if there is no 
    internet connection as it needs to call the cdb
    """
    def setUp(self): # pylint: disable=C0103, C0202
        """
        Run Simulation - temporarily store command line arguments somewhere
        else
        """
        self.temp_argv = sys.argv
        sys.argv = sys.argv[0:1]
        configuration = Configreader()
        server_name = configuration.cdb_download_url + \
                                            configuration.geometry_download_wsdl
        try:
            urllib2.urlopen(server_name)
        except Exception: # pylint: disable=W0703
            unittest.TestCase.skipTest(self, "No Internet Connection")

    def tearDown(self): # pylint: disable=C0103
        """Replace the system arguments"""
        sys.argv = self.temp_argv

    def test_get_geometry_ids_file(self): 
        """ Check that get_geometry_ids creates a file with the correct geoms"""
        run_get_geometries()
        geometry_id_file = os.path.join(TMP_PATH, "geometry_ids.txt")
        if os.path.isfile(geometry_id_file) == False:
            raise Exception('geometry ids not saved to file')
        fin = open(geometry_id_file, 'r')
        geometry_lines_found = 0
        for lines in fin.readlines():
            if lines.find('Geometry Number = 1') >= 0:
                geometry_lines_found += 1
            if lines.find('ValidFrom       = 2012-04-25 14:17:45.447614') \
                                                                           >= 0:
                geometry_lines_found += 1
            if lines.find('Date Created    = 2012-04-25 14:17:45.527000') \
                                                                           >= 0:
                geometry_lines_found += 1
        self.assertEqual(geometry_lines_found, 3, \
                           'First geometry not found check ' + geometry_id_file)

    def test_get_geometry_ids_nofile(self):
        """ Check that get_geometry_ids outputs the geometries """
        run_get_geometries()        
        geometry_id_file = os.path.join(TMP_PATH, "get_geometry_ids_output")
        fin = open(geometry_id_file, 'r')
        geometry_lines_found = 0
        for lines in fin.readlines():
            if lines.find('Geometry Number = 1') >= 0:
                geometry_lines_found += 1
            if lines.find('ValidFrom       = 2012-04-25 14:17:45.447614') \
                                                                           >= 0:
                geometry_lines_found += 1
            if lines.find('Date Created    = 2012-04-25 14:17:45.527000') \
                                                                           >= 0:
                geometry_lines_found += 1
        self.assertEqual(geometry_lines_found, 3, \
                           'First geometry not found check ' + geometry_id_file)
    
if __name__ == '__main__':
    unittest.main()
