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

"""Tests for MapPyMCReconSetup"""

import unittest
import json

import MapPyMCReconSetup

class TestMapPyReconSetup(unittest.TestCase):
    """Tests for MapPyMCReconSetup"""

    def setUp(self):
        """Initialises a MapPyMCReconSetup"""
        self.recon = MapPyMCReconSetup.MapPyMCReconSetup()

    def test_birth(self):
        """Should just always return true"""
        self.assertTrue(self.recon.birth(""))

    def test_death(self):
        """Should just always return true"""
        self.assertTrue(self.recon.death())

    def test_process_bad(self):
        """Should put in an errors branch"""
        json_out = json.loads(self.recon.process(""))
        self.assertTrue("errors" in json_out)
        json_out = json.loads(self.recon.process(json.dumps({})))
        self.assertFalse("errors" in json_out)
        json_out = json.loads(self.recon.process(json.dumps({"mc_events":None})))
        self.assertFalse("errors" in json_out)
    
    def test_process_good(self):
        """Should make the detector branches"""
        good = {"mc_events":[0]*3}
        spill = json.loads(self.recon.process(json.dumps(good)))
        self.assertEqual(len(spill["recon_events"]), 3)
        for i in range(3):
            ev = spill["recon_events"][i]
            self.assertEqual(ev["part_event_number"], i)
            self.assertEqual(ev["trigger_event"], {})
            self.assertEqual(ev["ckov_event"], {})
            self.assertEqual(ev["kl_event"], {})
            self.assertEqual(ev["emr_event"], {})
            self.assertEqual(ev["sci_fi_event"], {})
            self.assertEqual(ev["global_event"], {})
            self.assertEqual(ev["tof_event"], {})

if __name__ == "__main__":
    unittest.main()



