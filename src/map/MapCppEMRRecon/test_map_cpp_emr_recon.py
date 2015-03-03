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
        self.mapper.birth(self. c.getConfigJSON())

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
        n_ev = len(spill_out['recon_events'])
        self.assertEqual(2, n_ev)

	# check that the first event is empty, second contains 33 plane hits
        n_planes = len(spill_out['recon_events'][0]\
				  ['emr_event']['emr_plane_hits'])
        self.assertEqual(0, n_planes)

        n_planes = len(spill_out['recon_events'][1]\
				  ['emr_event']['emr_plane_hits'])
        self.assertEqual(33, n_planes)

	# consistent amount of primary bars (i.e. DBB hits close to trigger, 
	# 3 planes only contain fADC information without a bar hit)
        has_primary = spill_out['recon_events'][1]['emr_event']\
			       ['has_primary']

        self.assertEqual(True, has_primary)

        for i in range(0, n_planes): 
            n_primaries = len(spill_out['recon_events'][1]\
				       ['emr_event']['emr_plane_hits'][i]\
				       ['emr_bars_primary'])

            if (i < n_planes - 3):
                self.assertTrue(n_primaries == 1)
            else:
                self.assertTrue(n_primaries == 0)

	# make sure that the primary's ToT is the highest one of the plane
        for i in range(0, n_planes - 3):
            tot_primary = spill_out['recon_events'][1]\
				   ['emr_event']['emr_plane_hits'][i]\
				   ['emr_bars_primary'][0]['emr_bar_hits'][0]\
				   ['tot']

            n_bars = len(spill_out['recon_events'][1]\
				  ['emr_event']['emr_plane_hits'][i]\
				  ['emr_bars'])

            for j in range(0, n_bars):
                tot_i = spill_out['recon_events'][1]\
				['emr_event']['emr_plane_hits'][i]\
				['emr_bars'][j]['emr_bar_hits'][0]\
				['tot']

                self.assertTrue(tot_primary >= tot_i)


        # check that the e+ is associated with its mu+, 4 secondary plane hits
        has_secondary = spill_out['recon_events'][1]['emr_event']\
				 ['has_secondary']

        self.assertEqual(True, has_secondary)

        n_secondary_planes = 0
        for i in range(0, n_planes):
            n_hits = len(spill_out['recon_events'][1]\
				   ['emr_event']['emr_plane_hits'][i]\
				   ['emr_bars_secondary'])
            if (n_hits > 0):
                n_secondary_planes += 1

        self.assertEqual(4, n_secondary_planes)

        # check that the primary coordinates are well reconstructed
        for i in range(0, n_planes - 3):
            plane_id = spill_out['recon_events'][1]['emr_event']\
				['emr_plane_hits'][i]['plane']
				
            bar_id = spill_out['recon_events'][1]\
			      ['emr_event']['emr_plane_hits'][i]\
                              ['emr_bars_primary'][0]['bar']

            x_i = spill_out['recon_events'][1]['emr_event']\
		          ['emr_plane_hits'][i]['emr_bars_primary'][0]\
			  ['emr_bar_hits'][0]['x']
            y_i = spill_out['recon_events'][1]['emr_event']\
		          ['emr_plane_hits'][i]['emr_bars_primary'][0]\
			  ['emr_bar_hits'][0]['y']
            z_i = spill_out['recon_events'][1]['emr_event']\
		          ['emr_plane_hits'][i]['emr_bars_primary'][0]\
			  ['emr_bar_hits'][0]['z']

            self.assertTrue(z_i > plane_id*17.5)
            self.assertTrue(z_i < (plane_id+1)*17.5)

            if (plane_id % 2 == 0):
                self.assertTrue(x_i > (bar_id - 30 - 1)*17)
                self.assertTrue(x_i < (bar_id - 30 + 1)*17)
            else:
                self.assertTrue(y_i > (bar_id - 30 - 1)*17)
                self.assertTrue(y_i < (bar_id - 30 + 1)*17)

        # consistency of the ranges and the distance between the two tracks
        range_primary = spill_out['recon_events'][1]['emr_event']\
				 ['range_primary']

        self.assertTrue(range_primary > plane_id*17.5)
        self.assertTrue(range_primary < (plane_id+1)*17.5)

        range_secondary = spill_out['recon_events'][1]['emr_event']\
				   ['range_secondary']

        self.assertTrue(range_secondary > 0)

        track_distance = spill_out['recon_events'][1]['emr_event']\
				   ['secondary_to_primary_track_distance']

        self.assertTrue(track_distance < 80)

		
if __name__ == "__main__":
    unittest.main()

