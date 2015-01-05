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
        # test with a mu+, its decay product e+ and a noise hit (e-)
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)

        self.assertFalse("MapCppEMRRecon" in spill_out["errors"])

	# consistent amount of reconEvents (4 = 1primary+1noise+1decay+1secondary)
        n_ev = len(spill_out['recon_events'])
        self.assertEqual(4, n_ev)

	# unchanged amount of primary plane hits, charge and ToTs
        n_hits_0 = len(spill_out['recon_events'][0]['emr_event']\
				['emr_plane_hits'])
        self.assertEqual(24, n_hits_0)
        for i in range(0, n_hits_0):
            self.assertTrue(spill_out['recon_events'][0]['emr_event']\
				     ['emr_plane_hits'][i]['charge'] > 0)
            self.assertTrue(spill_out['recon_events'][0]['emr_event']\
				     ['emr_plane_hits'][i]['emr_bars'][0]\
				     ['emr_bar_hits'][0]['tot'] > 5)
	# unchanged amount of noise and time from the primary
        n_hits_1 = len(spill_out['recon_events'][1]['emr_event']\
				['emr_plane_hits'])
        self.assertEqual(1, n_hits_1)
        self.assertTrue(spill_out['recon_events'][1]['emr_event']\
				 ['emr_plane_hits'][0]['emr_bars'][0]\
				 ['emr_bar_hits'][0]['delta_t'] - spill_out\
				 ['recon_events'][0]['emr_event']\
				 ['emr_plane_hits'][0]['emr_bars'][0]\
				 ['emr_bar_hits'][0]['delta_t'] > 20)
	# unchanged amount of decay plane hits and bar hits in each plane 
        n_hits_2 = len(spill_out['recon_events'][2]['emr_event']\
				['emr_plane_hits'])
        self.assertEqual(8, n_hits_2)
        for i in range(0, n_hits_2):
            n_bars = len(spill_out['recon_events'][2]['emr_event']\
				  ['emr_plane_hits'][i]['emr_bars'])
            self.assertTrue(2, n_bars)

	# correct number of planes hit in the secondary event (1 track, 8 planes)
        n_hits_3 = len(spill_out['recon_events'][3]['emr_event']\
				['emr_plane_hits'])
        self.assertEqual(8, n_hits_3)
        # only one hit in each plane in cleaned array
        for i in range(0, n_hits_3):
            self.assertEqual(len(spill_out['recon_events'][3]['emr_event']\
				 ['emr_plane_hits'][i]['emr_bars_primary']\
				 [0]['emr_bar_hits']), 1)
        # coordinate reconstructed and consistent
        for i in range(0, n_hits_3 - 1):
            x_1 =  spill_out['recon_events'][3]['emr_event']\
			    ['emr_plane_hits'][i]['emr_bars_primary'][0]\
		            ['emr_bar_hits'][0]['x']
            x_2 =  spill_out['recon_events'][3]['emr_event']\
			    ['emr_plane_hits'][i+1]['emr_bars_primary'][0]\
		            ['emr_bar_hits'][0]['x']
            self.assertTrue(x_1 > 0)
            self.assertTrue(x_2 > 0)
            self.assertTrue(x_2 - x_1 < 2)

if __name__ == "__main__":
    unittest.main()

