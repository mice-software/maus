# This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

"""Tests for ReducePyTofCalib"""
# pylint: disable = C0103

import os
import json
import unittest
from Configuration import Configuration
import MAUS

class ReducePyTofCalibTestCase(unittest.TestCase):# pylint: disable = R0904
    """Tests for ReducePyTofCalib"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """init a ReducePyTofCalib object"""
        cls.reducer = MAUS.ReducePyTofCalib()
        cls.c = Configuration()

    def test_empty(self):
        """Check against configuration is empty"""
        # result = self.reducer.birth("")
        # self.assertFalse(result)
        result = self.reducer.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        errors = doc["errors"]["ReducePyTofCalib"]
        self.assertTrue("No JSON object could be decoded" in errors)

    def test_init(self):
        """Check that birth works properly"""
        success = self.reducer.birth(self. c.getConfigJSON())
        self.assertTrue(success)

    def test_no_data(self):
        """Check that against data stream is empty"""
        noDigit_json = {"run_number": 1, "maus_event_type": "Spill", "recon_events": [], "spill_number": 0, "errors": {}, "daq_event_type": "physics_event", "daq_data": {}} # pylint: disable=C0301
        result = self.reducer.process(json.dumps(noDigit_json))
        spill = json.loads(result)
        self.assertTrue('slab_hits' not in spill)

    def test_process(self):
        """Check ReducePyTofCalib process function"""
        test2 = ('%s/src/reduce/ReducePyTofCalib/processTest.json' %
                                                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with some crazy events.
        self.reducer.process(data)

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that death works ok"""
        success = cls.reducer.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        cls.reducer = None

if __name__ == '__main__':
    unittest.main()
