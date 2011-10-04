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

import os
import subprocess
import unittest
import json

MAUS_ROOT_DIR = os.getenv("MAUS_ROOT_DIR")
ANA_PATH = os.path.join(MAUS_ROOT_DIR, "bin", "analyze_data_offline.py")
OUT_PATH = os.path.join(MAUS_ROOT_DIR, "tmp", "analysis.out")

class AnalyzeOfflineTest(unittest.TestCase): # pylint: disable = R0904
    """
    Tests for the offline analysis exe
    """

    def test_default_running(self): #pylint: disable=R0201
        """
        Check that the default parameters produce sensible output
        """
        subproc = subprocess.Popen([ANA_PATH, "-output_json_file_name",
                                    OUT_PATH])
        subproc.wait()
        fin = open(OUT_PATH)
        for line in fin.readlines():
            json.loads(line)
        fin.close()

if __name__ == "__main__":
    unittest.main()


