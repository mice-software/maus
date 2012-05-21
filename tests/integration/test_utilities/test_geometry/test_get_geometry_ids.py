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
    Run get_ids to generate some data. We only want to do this once, so I
    pull it out into a separate part of the test.
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
    This class has two tests. One checks that particles are accumulated into one
    vrml output file when the tag is selected in configuration defaults. The 
    second checks that the DrawByParticleID model has been registered so 
    particles will have specified colours in the vrml.
    """
    def setUp(self): # pylint: disable=C0103, C0202
        """ Run Simulation """
        self.internet_connection = True
        self.configuration = Configreader()
        self.server_name = self.configuration.cdb_download_url + \
                                       self.configuration.geometry_download_wsdl
        try:
            urllib2.urlopen(self.server_name)
        except urllib2.URLError:
            self.internet_connection == False
            unittest.TestCase.skipTest(self, "No Internet Connection")
        if self.internet_connection == True:
            run_get_geometries()

    def test_get_geometry_ids_file(self): 
        """ Check that get_geometry_ids creates a file with the correct geoms"""
        if self.internet_connection == False:
            print "SKIP test no internet connection"
        else:
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
        if self.internet_connection == False:
            print "SKIP test no internet connection"
        else:
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
