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

class TestMapPyReconSetup(unittest.TestCase): #pylint:disable=R0904
    """Tests for MapPyMCReconSetup"""

    def setUp(self): #pylint: disable=C0103
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
        json_out = json.loads(self.recon.process\
                                               (json.dumps({"mc_events":None})))
        self.assertFalse("errors" in json_out)
    
    def test_process_good(self):
        """Should make the detector branches"""
        good = {"mc_events":[0]*3}
        spill = json.loads(self.recon.process(json.dumps(good)))
        self.assertEqual(len(spill["recon_events"]), 3)
        for i in range(3):
            event = spill["recon_events"][i]
            self.assertEqual(event["part_event_number"], i)
            self.assertEqual(event["trigger_event"], {})
            self.assertEqual(event["ckov_event"], {})
            self.assertEqual(event["kl_event"], {})
            self.assertEqual(event["emr_event"], {})
            self.assertEqual(event["sci_fi_event"], {})
            self.assertEqual(event["global_event"], {})
            self.assertEqual(event["tof_event"], {})

if __name__ == "__main__":
    unittest.main()



