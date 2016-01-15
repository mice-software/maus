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

"""Tests for MapCppEMRRecon"""

import os
import json
import unittest
from Configuration import Configuration
import MAUS
import maus_cpp.converter

class TestMapCppEMRRecon(unittest.TestCase): #pylint: disable=R0904
    """Tests for MapCppEMRRecon"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MAUS.MapCppEMRRecon()
        cls.c = Configuration()

    def test_empty(self):
        """Check can handle empty configuration"""
        self.assertRaises(ValueError, self.mapper.birth, "")
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppEMRRecon" in doc["errors"])

    def test_init(self):
        """Check birth with default configuration"""
        test_configuration = self.c.getConfigJSON()
        test_conf_json = json.loads(test_configuration)
        # Fix the calibration to be always step I (prevents update issues)
        test_conf_json['EMR_calib_source'] = "file"
        test_conf_json['EMR_calib_file'] = \
            "/files/calibration/emrcalib_cosmics_july2015.txt"
        test_conf = json.dumps(test_conf_json)
        self.mapper.birth(test_conf)

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppEMRRecon/noDataTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse("MapCppEMRRecon" in spill_out["errors"])

        n_ev = len(spill_out['recon_events'])
        self.assertEqual(0, n_ev)

    def test_process(self):
        """Test MapCppEMRRecon process method"""
        test2 = ('%s/src/map/MapCppEMRRecon/processTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with a mu+ and its decay product e+
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)

        self.assertFalse("MapCppEMRRecon" in spill_out["errors"])

	# consistent amount of reconEvents (2 = empty + muon and its decay)
        self.assertEqual(2, len(spill_out['recon_events']))

	# check that the first event contains 40 space points, second is empty
        self.assertEqual(40, len(spill_out['recon_events'][0]['emr_event']\
					  ['event_tracks'][0]['space_points']))
        self.assertEqual(0, len(spill_out['recon_events'][1]['emr_event']\
					 ['event_tracks']))

        # check that the density is reconstructed and consistent for the mother
        self.assertEqual(spill_out['recon_events'][0]['emr_event']\
			          ['event_tracks'][0]['plane_density_ma'], 1.)
        self.assertEqual(spill_out['recon_events'][0]['emr_event']\
			          ['event_tracks'][0]['plane_density_sa'], 1.)

        # check that the event charge and charge ratio are reconstructed
        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['total_charge_ma'] > 1000)
        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['total_charge_sa'] > 1000)

        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['charge_ratio_ma'] < .5)
        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['charge_ratio_sa'] < .5)

        # check the fitting paremeters are reconstructed and the chi^2
        self.assertTrue(2, spill_out['recon_events'][0]['emr_event']\
				    ['event_tracks'][0]['track']['parx'])
        self.assertTrue(2, spill_out['recon_events'][0]['emr_event']\
				    ['event_tracks'][0]['track']['pary'])

        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
				 ['event_tracks'][0]['track']['chi2'] < 2)

        # check that we have have 1 track point per SP + the origin (40+1)
        self.assertEqual(41, len(spill_out['recon_events'][0]['emr_event']\
					  ['event_tracks'][0]['track']\
					  ['track_points']))
        for i in range(0, 41):
            self.assertTrue(spill_out['recon_events'][0]['emr_event']\
		                     ['event_tracks'][0]['track']\
			             ['track_points'][i]['resx'] < 17)
            self.assertTrue(spill_out['recon_events'][0]['emr_event']\
		                     ['event_tracks'][0]['track']\
			             ['track_points'][i]['resy'] < 17)

        # check that the range and momentum are good (MC truth = 194 Mev/c)
        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['track']['range'] > 300)
        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['track']['range'] < 500)

        mom_truth = spill_out['mc_events'][0]['primary']['momentum']['z']
        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['track']['mom']
				 > mom_truth-5)
        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][0]['track']['mom']
				 < mom_truth+5)

        # check that the electron has been matched to the muon
        self.assertEqual(len(spill_out['recon_events'][0]['emr_event']\
			              ['event_tracks']), 2)

        # check that it contains everything
        n_planes = len(spill_out['recon_events'][0]['emr_event']\
			        ['event_tracks'][1]['plane_hits'])
        self.assertEqual(n_planes, 3)
        n_bars = 0
        for i in range(0, n_planes):
            n_bars = n_bars + len(spill_out['recon_events'][0]['emr_event']\
			                   ['event_tracks'][1]['plane_hits']\
					   [i]['bar_hits'])
        self.assertEqual(n_planes, 3)
        self.assertEqual(len(spill_out['recon_events'][0]['emr_event']\
			              ['event_tracks'][1]['space_points']),\
			 n_bars)
        self.assertEqual(len(spill_out['recon_events'][0]['emr_event']\
			              ['event_tracks'][1]['track']\
				      ['track_points']), n_bars)

        # check that the positron momentum is consitent with a Michel electron
        self.assertTrue(spill_out['recon_events'][0]['emr_event']\
			         ['event_tracks'][1]['track']['mom'] < 55)


if __name__ == "__main__":
    unittest.main()

