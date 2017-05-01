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
Tests for the offline analysis exe
"""

import sys
import os
import subprocess
import unittest
import json

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
ANA_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "analyze_data_offline.py")
OUT_PATH = os.path.join(MAUS_ROOT_DIR, "tmp", "test_analyze_data_offline")
CONV_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "utilities", "root_to_json.py")

class AnalyzeOfflineTest(unittest.TestCase): # pylint: disable = R0904
    """
    Tests for the offline analysis exe
    """

    def test_default_running(self): #pylint: disable=R0201
        """
        Check that the default parameters produce sensible output
        """
        try:
            os.remove(OUT_PATH)
        except OSError:
            pass
        if "valgrind" in sys.argv:
            valgrind = ['valgrind', '--leak-check=yes',]
        else:
            valgrind = []
        subproc = subprocess.Popen(valgrind+['python', ANA_PATH,
                                    "--output_root_file_name", OUT_PATH+".root",
                                    "--daq_data_file=6008",
                                    "--DAQ_cabling_by", "date",
                                    "--TOF_cabling_by", "date",
                                    "--TOF_calib_by", "date"])
        subproc.wait()
        self.assertEqual(subproc.returncode, 0)
        subproc = subprocess.Popen([CONV_PATH,
                                    "-input_root_file_name", OUT_PATH+".root",
                                    "-output_json_file_name", OUT_PATH+".json"])
        subproc.wait()
        self.assertEqual(subproc.returncode, 0)
        fin = open(OUT_PATH+".json")
        all_lines = fin.readlines()
        self.assertGreater(len(all_lines), 0)
        for line in all_lines:
            json.loads(line)
        fin.close()

if __name__ == "__main__":
    unittest.main()


