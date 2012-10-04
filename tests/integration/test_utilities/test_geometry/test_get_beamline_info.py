"""
test_get_beamline_info.py
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
                                                         "get_beamline_info.py")
TEST_DIR = os.path.join(MAUS_ROOT_DIR, "tests", "integration", \
                        "test_utilities", "test_geometry")
CONFIG_PATH_RUN = os.path.join(TEST_DIR, "test_get_beamline_info_run.txt")
CONFIG_PATH_DATES = os.path.join(TEST_DIR, "test_get_beamline_info_dates.txt")
INTERNET_CONNECTION = True

def run_get_beamlines(run_dates):
    """
    Run get_beamline_info to generate some data.
    """
    if run_dates == 'run':
        file_name = os.environ['MAUS_ROOT_DIR']+\
                                             '/tmp/get_beamline_info_run_output'
        test_out = open(file_name, 'w')
        subproc = subprocess.Popen([SIM_PATH, \
                               '-configuration_file', CONFIG_PATH_RUN],\
                                      stdout=test_out, stderr=subprocess.STDOUT)
        subproc.wait()
        test_out.close()
    elif run_dates == 'dates':
        file_name = os.environ['MAUS_ROOT_DIR']+\
                                           '/tmp/get_beamline_info_dates_output'
        test_out = open(file_name, 'w')
        subproc = subprocess.Popen([SIM_PATH, \
                               '-configuration_file', CONFIG_PATH_DATES],\
                                      stdout=test_out, stderr=subprocess.STDOUT)
        subproc.wait()
        test_out.close()
            
class TestGetBeamlineInfo(unittest.TestCase): #pylint:disable= R0904
    """
    This test calls the get_beamline_info specifying the executable to
    both print to screen the run number and time fram calls. It captures
    the screen print to file and checks the correct information can
    be found within it. It will not carry out the test if there is no 
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

    def test_get_beamline_info_run(self):
        """ Check that get_beamline_info outputs the info by run """
        run_get_beamlines('run')     
        geometry_id_file = os.path.join(TMP_PATH, \
                                                 "get_beamline_info_run_output")
        fin = open(geometry_id_file, 'r')
        geometry_lines_found = 0
        for lines in fin.readlines():
            if lines.find('Server status is OK') >= 0:
                geometry_lines_found += 1
        self.assertEqual(geometry_lines_found, 1, \
                           'The CDB was not queried, no beamline found')

    def test_get_beamline_info_dates(self):
        """ Check that get_beamline_info outputs the info by dates """
        run_get_beamlines('dates')     
        geometry_id_file = os.path.join(TMP_PATH, \
                                               "get_beamline_info_dates_output")
        fin = open(geometry_id_file, 'r')
        geometry_lines_found = 0
        for lines in fin.readlines():
            if lines.find('Server status is OK') >= 0:
                geometry_lines_found += 1
        self.assertEqual(geometry_lines_found, 1, \
                           'The CDB was not queried, no beamlines found')
    
if __name__ == '__main__':
    unittest.main()
