"""
M. Littlefield
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

import os
import unittest
from geometry.ConfigReader import Configreader

class test_config_reader(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    This class tests the module ConfigReader.py
    which reads the configuration defaults and makes
    the info available for the geometry files. This test
    ensures it is doing what it is expected to.
    """
    def setUp(self): #pylint: disable = C0103
        """
        method set_up This method creates a Configreader
        object ready to test
        """
        self.testcase = Configreader()

    def test_readconfig(self):
        """
        These tests work by reading the defaults from
        the ConfigurationDeafaults(Geometry) and compares them
        to what is expected in this file.
        """
        self.testcase.readconfig()
        cdb_download_url = "http://cdb.mice.rl.ac.uk/cdb/"
        cdb_upload_url = "http://cdb.mice.rl.ac.uk/cdb/"
        cdb_download_geometry_dir = "geometry?wsdl"
        cdb_upload_geometry_dir = "geometrySuperMouse?wsdl"
        geometry_upload_directory   = "%s/files/geometry/upload" % os.environ.get("MAUS_ROOT_DIR")
        geometry_download_directory   = "%s/files/geometry/download" % os.environ.get("MAUS_ROOT_DIR")
        geometry_description = "This is a standard note It uses the standard test case"
        geometry_zip_file         = 0
        geometry_delete_originals = 0
        geometry_start_time = "2011-09-08 09:00:00"
        geometry_stop_time  = None
        geometry_run_number = 1
        self.assertEqual(self.testcase.cdb_dl_url, cdb_download_url)
        self.assertEqual(self.testcase.cdb_ul_url, cdb_upload_url)
        self.assertEqual(self.testcase.cdb_dl_dir, cdb_download_geometry_dir)
        self.assertEqual(self.testcase.cdb_ul_dir, cdb_upload_geometry_dir)
        self.assertEqual(self.testcase.maus_ul_dir, geometry_upload_directory)
        self.assertEqual(self.testcase.maus_dl_dir, geometry_download_directory)
        self.assertEqual(self.testcase.geometrynotes, geometry_description)
        self.assertEqual(self.testcase.zipfile, geometry_zip_file)
        self.assertEqual(self.testcase.deleteoriginals, geometry_delete_originals)
        self.assertEqual(self.testcase.starttime, geometry_start_time)
        self.assertEqual(self.testcase.stoptime, geometry_stop_time)
        self.assertEqual(self.testcase.runnum, geometry_run_number)

if __name__ == '__main__':
    unittest.main()
