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

"""Tests for MapPyReconSetup"""

import unittest
import json

import MapPyReconSetup

class TestMapPyReconSetup(unittest.TestCase):
    """Tests for MapPyTriggerRecon"""

    def setUp(self):
        self.recon = MapPyReconSetup.MapPyReconSetup()

    def test_birth(self):
        self.assertTrue(self.recon.birth(""))

    def test_death(self):
        self.assertTrue(self.recon.death())

    def test_process_bad(self):
        json_out = json.loads(self.recon.process(""))
        self.assertTrue("errors" in json_out)
        json_out = json.loads(self.recon.process(json.dumps({})))
        self.assertFalse("errors" in json_out)
        json_out = json.loads(self.recon.process(json.dumps({"daq_data":None})))
        self.assertFalse("errors" in json_out)
        json_out = json.loads(self.recon.process(json.dumps({"daq_data":{}})))
        self.assertFalse("errors" in json_out)
        json_out = json.loads(self.recon.process(json.dumps({"daq_data":{
            "trigger_event":[]}})))
        self.assertFalse("errors" in json_out)
    
    def __v1290(self, an_int):
        return {"V1290":[
                {"part_event_number":an_int},
                {"part_event_number":an_int},
                {"part_event_number":an_int},
                {"part_event_number":an_int},
        ]}

    def test_process_good(self):
        trigs = [23, 17, 42]
        good = {"daq_data":{"trigger":[self.__v1290(x) for x in trigs]}}
        spill = json.loads(self.recon.process(json.dumps(good)))
        self.assertEqual(len(spill["recon_events"]), 3)
        for i in range(3):
            ev = spill["recon_events"][i]
            self.assertEqual(ev["part_event_number"], trigs[i])
            self.assertEqual(ev["trigger_event"], {})
            self.assertEqual(ev["ckov_event"], {})
            self.assertEqual(ev["kl_event"], {})
            self.assertEqual(ev["emr_event"], {})
            self.assertEqual(ev["scifi_event"], {})
            self.assertEqual(ev["global_event"], {})
            self.assertEqual(ev["tof_event"], {})

if __name__ == "__main__":
    unittest.main()

