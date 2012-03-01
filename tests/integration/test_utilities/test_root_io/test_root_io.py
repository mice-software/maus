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
Tests for the json_to_root and root_to_json utilities
"""

import unittest
import os
import subprocess
import json

class TestRootIO(unittest.TestCase): #pylint: disable=R0904
    """
    Tests for the json_to_root and root_to_json utilities
    """

    def setUp(self):
        """Write some test output data"""
        self.test_data = {
            "scalars":{},
            "emr_spill_data":{},
            "daq_data":{},
            "spill_number":1,
            "recon_events":[],
            "mc_events":[]
        }
        self.json_name = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp",
                                                       "json_to_root_test.json")
        self.root_name = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp",
                                                       "root_to_json_test.root")
        self.json_to_root = os.path.join(os.environ["MAUS_ROOT_DIR"], "bin",
                                                 "utilities", "json_to_root.py")
        self.root_to_json = os.path.join(os.environ["MAUS_ROOT_DIR"], "bin",
                                                 "utilities", "root_to_json.py")
        fout = open(self.json_name, 'w')
        for i in range(10):
            print >> fout, json.dumps(self.test_data)
        print >> fout

    def test_all(self):
        """Try writing a root file then reading it back"""
        subproc = subprocess.Popen \
                ([self.json_to_root, '-input_json_file_name', self.json_name,
                                     '-output_root_file_name', self.root_name,
                                     '-verbose_level', '0'])
        subproc.wait()
        subproc = subprocess.Popen \
                ([self.root_to_json, '-input_root_file_name', self.root_name,
                                     '-output_json_file_name', self.json_name])
        subproc.wait()
        
    

if __name__ == "__main__":
    unittest.main()


