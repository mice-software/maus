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

"""Tests for MapCppEMRSpacePoints"""

import os
import json
import unittest
from Configuration import Configuration
import MAUS
import maus_cpp.converter

class TestMapCppEMRSpacepoints(unittest.TestCase): #pylint: disable=R0904
    """Tests for MapCppEMRSpacePoints"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MAUS.MapCppEMRSpacePoints()
        cls.c = Configuration()

    def test_empty(self):
        """Check can handle empty configuration"""
        self.assertRaises(ValueError, self.mapper.birth, "")
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppEMRSpacePoints" in doc["errors"])

    def test_init(self):
        """Check birth with default configuration"""
        test_configuration = self.c.getConfigJSON()
        test_conf_json = json.loads(test_configuration)
        # Fix the calibration to be always step IV (prevents changes issues)
        test_conf_json['EMR_calib_source'] = "file"
        test_conf_json['EMR_calib_file'] = \
            "/files/calibration/emrcalib_cosmics_july2015.txt"
        test_conf = json.dumps(test_conf_json)
        self.mapper.birth(test_conf)

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppEMRSpacePoints/noDataTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse("MapCppEMRSpacePoints" in spill_out["errors"])

        n_ev = len(spill_out['recon_events'])
        self.assertEqual(0, n_ev)

    def test_process(self):
        """Test MapCppEMRSpacePoints process method"""
        test2 = ('%s/src/map/MapCppEMRSpacePoints/processTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with a mu+ and its decay product e+
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)

        self.assertFalse("MapCppEMRSpacePoints" in spill_out["errors"])

	# consistent amount of reconEvents (2 = empty + muon and its decay)
        self.assertEqual(2, len(spill_out['recon_events']))

	# check that the first event contains 23 planes, second is empty
        self.assertEqual(23, len(spill_out['recon_events'][0]['emr_event']\
					  ['event_tracks'][0]['plane_hits']))
        self.assertEqual(0, len(spill_out['recon_events'][1]['emr_event']\
					  ['event_tracks']))

        # check that we have a decay candiate at the spill level
        self.assertEqual(1, len(spill_out['emr_spill_data']['event_tracks']))

        # check that there is as many space points as there are bar hits
        n_bars_muon = 0
        for i in range(0, len(spill_out['recon_events'][0]['emr_event']\
				       ['event_tracks'][0]['plane_hits'])):
            n_bars_muon = n_bars_muon\
                          + len(spill_out['recon_events'][0]['emr_event']\
				         ['event_tracks'][0]['plane_hits']\
					 [i]['bar_hits'])
        self.assertEqual(len(spill_out['recon_events'][0]['emr_event']\
			              ['event_tracks'][0]['space_points']),\
		         n_bars_muon)

        n_bars_pos = 0
        for i in range(0, len(spill_out['emr_spill_data']['event_tracks'][0]\
				       ['plane_hits'])):
            n_bars_pos = n_bars_pos\
                         + len(spill_out['emr_spill_data']['event_tracks'][0]\
				        ['plane_hits'][i]['bar_hits'])
        self.assertEqual(len(spill_out['emr_spill_data']['event_tracks'][0]\
			              ['space_points']), n_bars_pos)

        # check that all the position of the spacepoints are believable
        for i in range(0, n_bars_muon):
            chn = spill_out['recon_events'][0]['emr_event']['event_tracks']\
		           [0]['space_points'][i]['ch']
            xcoord = spill_out['recon_events'][0]['emr_event']['event_tracks']\
			    [0]['space_points'][i]['pos']['x']
            ycoord = spill_out['recon_events'][0]['emr_event']['event_tracks']\
			      [0]['space_points'][i]['pos']['y']
            zcoord = spill_out['recon_events'][0]['emr_event']['event_tracks']\
			      [0]['space_points'][i]['pos']['z']
            if not (chn//60)%2:
                self.assertTrue(xcoord > (chn%60-31)*17)
                self.assertTrue(xcoord < (chn%60-29)*17)
            else:
                self.assertTrue(ycoord > (chn%60-31)*17)
                self.assertTrue(ycoord < (chn%60+29)*17)
            self.assertTrue(zcoord > (chn//60-24)*17.5)
            self.assertTrue(zcoord < (chn//60+1-24)*17.5)

        for i in range(0, n_bars_pos):
            chn = spill_out['emr_spill_data']['event_tracks'][0]\
		           ['space_points'][i]['ch']
            xcoord = spill_out['emr_spill_data']['event_tracks'][0]\
			      ['space_points'][i]['pos']['x']
            ycoord = spill_out['emr_spill_data']['event_tracks'][0]\
			      ['space_points'][i]['pos']['y']
            zcoord = spill_out['emr_spill_data']['event_tracks'][0]\
			      ['space_points'][i]['pos']['z']
            if not (chn//60)%2:
                self.assertTrue(xcoord > (chn%60-31)*17)
                self.assertTrue(xcoord < (chn%60-29)*17)
            else:
                self.assertTrue(ycoord > (chn%60-31)*17)
                self.assertTrue(ycoord < (chn%60+29)*17)
            self.assertTrue(zcoord > (chn//60-24)*17.5)
            self.assertTrue(zcoord < (chn//60+1-24)*17.5)

	# check that the charge is reconstructed for the muon and its decay
        for i in range(0, n_bars_muon):
            charge_sa = spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['space_points'][i]\
			         ['charge_sa']
            charge_ma = spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['space_points'][i]\
			         ['charge_ma']
            self.assertTrue(charge_sa > 0)
            self.assertTrue(charge_ma > 0)

        for i in range(0, n_bars_pos):
            charge_sa = spill_out['emr_spill_data']['event_tracks'][0]\
			         ['space_points'][i]['charge_sa']
            charge_ma = spill_out['emr_spill_data']['event_tracks'][0]\
			         ['space_points'][i]['charge_ma']
            self.assertTrue(charge_sa > 0)
            self.assertTrue(charge_ma > 0)

if __name__ == "__main__":
    unittest.main()

