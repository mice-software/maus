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

"""Tests for MapCppEMRMCDigitization"""

import os
import json
import unittest
from Configuration import Configuration
import MAUS
import maus_cpp.converter

class TestMapCppEMRMCDigitization(unittest.TestCase): #pylint: disable=R0904
    """Tests for MapCppEMRMCDigitization"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MAUS.MapCppEMRMCDigitization()
        cls.c = Configuration()

    def test_empty(self):
        """Check can handle empty configuration"""
        self.assertRaises(ValueError, self.mapper.birth, "")
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppEMRMCDigitization" in doc["errors"])

    def test_init(self):
        """Check birth with default configuration"""
        test_configuration = self.c.getConfigJSON()
        test_conf_json = json.loads(test_configuration)
        # Fix the calibration to be always step IV (prevents update issues)
        test_conf_json['EMR_calib_source'] = "file"
        test_conf_json['EMR_calib_file'] = \
            "/files/calibration/emrcalib_cosmics_july2015.txt"
        test_conf = json.dumps(test_conf_json)
        self.mapper.birth(test_conf)

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppEMRMCDigitization/noDataTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse("MapCppEMRMCDigitization" in spill_out["errors"])

        n_ev = len(spill_out['recon_events'])
        self.assertEqual(0, n_ev)

    def test_process(self):
        """Test MapCppEMRMCDigitization process method"""
        test2 = ('%s/src/map/MapCppEMRMCDigitization/processTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with a mu+, its decay product e+ and a noise hit (e-)
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse("MapCppEMRMCDigitization" in spill_out["errors"])

	# consistent amount of reconEvents (1primary)
        n_ev = len(spill_out['recon_events'])
        self.assertEqual(1, n_ev)

	# consistent amount of primary plane hits, charge and ToTs
        n_planes = len(spill_out['recon_events'][0]['emr_event']\
				['event_tracks'][0]['plane_hits'])
        self.assertEqual(24, n_planes)
        for i in range(0, n_planes):
            self.assertTrue(spill_out['recon_events'][0]['emr_event']\
				     ['event_tracks'][0]['plane_hits'][i]\
				     ['charge'] > 0)
            n_hits = len(spill_out['recon_events'][0]['emr_event']\
				  ['event_tracks'][0]['plane_hits'][i]\
				  ['bar_hits'])
            for j in range(0, n_hits):
                self.assertTrue(spill_out['recon_events'][0]['emr_event']\
				         ['event_tracks'][0]['plane_hits'][i]\
				         ['bar_hits'][j]['tot'] > 0)

        # hits stored at the spill level (all of the hits)
        n_spill_hits = len(spill_out['emr_spill_data']['bar_hits'])
        self.assertEqual(50, n_spill_hits)

        # the noise hit is rejected as its by itself (no SP possible)
        self.assertEqual(1, len(spill_out['emr_spill_data']['event_tracks']))
        for i in range(0, len(spill_out['emr_spill_data']['event_tracks'][0]\
				       ['plane_hits'])):
            self.assertEqual(spill_out['emr_spill_data']['event_tracks'][0]\
				      ['plane_hits'][i]['charge'], -1.0)
            n_hits = len(spill_out['emr_spill_data']['event_tracks'][0]\
				  ['plane_hits'][i]['bar_hits'])
            for j in range(0, n_hits):
                self.assertTrue(spill_out['emr_spill_data']['event_tracks'][0]\
				         ['plane_hits'][i]\
					 ['bar_hits'][j]['tot'] > 0)

if __name__ == "__main__":
    unittest.main()

