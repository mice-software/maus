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

"""test geometry validation script"""

import os
import unittest
import subprocess
import time
import json

TEST = os.path.expandvars("${MAUS_TMP_DIR}/geometry_validation.json")
VOL_REF = [u'DummyPV', u'Vol1', u'Vol11', u'Vol12', u'Vol13', u'Vol14',
    u'Vol15', u'Vol16', u'Vol17', u'Vol18', u'Vol19', u'Vol2', u'Vol20',
    u'Vol3', u'Vol4', u'Vol5', u'Vol6', u'Vol7', u'Vol8', u'Vol9']

def run_subprocess():
    """Run the geometry validation"""
    exe = os.path.expandvars("${MAUS_ROOT_DIR}/bin/utilities/"+\
                                 "geometry_validation.py")
    cards = os.path.expandvars("${MAUS_ROOT_DIR}/tests/integration/"+\
                          "test_utilities/test_geometry_validation/test_cards")
    print exe
    proc = subprocess.Popen([exe, "--configuration_file", cards],
                            stdin=subprocess.PIPE)
    while proc.poll() == None: # and __name__ != "__main__":
        proc.communicate('\n')
        time.sleep(1)
    proc.wait()
    return proc.returncode

class TestGeometryValidation(unittest.TestCase): #pylint: disable=R0904
    """test geometry validation script"""
    def test_geometry_validation(self):
        """test geometry validation script"""
        self.assertEqual(run_subprocess(), 0)
        lines = [line for line in open(TEST).readlines()]
        n_steps = 3
        self.assertEqual(len(lines), n_steps)
        first_event = json.loads(lines[0])[0]["tracks"][0]
        self.assertAlmostEqual(first_event["initial_position"]["x"], -1.)
        self.assertAlmostEqual(first_event["initial_position"]["y"], 2.)
        self.assertAlmostEqual(first_event["initial_position"]["z"], -500.)
        self.assertGreater(len(first_event["steps"]), 1)
        last_event = json.loads(lines[-1])[0]["tracks"][0]
        self.assertAlmostEqual(last_event["initial_position"]["x"],
                               -1.+1.*(n_steps-1))
        self.assertAlmostEqual(last_event["initial_position"]["y"],
                               2.-2.*(n_steps-1))
        self.assertAlmostEqual(last_event["initial_position"]["z"], -500.)
        self.assertGreater(len(last_event["steps"]), 1)
        for _format in ["ps", "png"]:
            for pic in ["test_geometry_validation_materials_1d",
                        "test_geometry_validation_materials_2d",
                        "test_geometry_validation_volumes_1d",
                        "test_geometry_validation_volumes_2d",
                       ]:
                fname = os.path.expandvars("${MAUS_TMP_DIR}/"+pic+"."+_format)
                self.assertTrue(os.path.exists(fname), msg=fname)
        fin = open(os.path.expandvars(
                    "${MAUS_TMP_DIR}/test_geometry_validation_volumes.json"))
        bbs = json.loads(fin.read())
        vol_list = sorted(bbs.keys())
        self.assertEqual(vol_list, VOL_REF)


if __name__ == "__main__":
    unittest.main()

