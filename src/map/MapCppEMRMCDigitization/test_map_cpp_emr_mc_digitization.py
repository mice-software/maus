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
        self.mapper.birth(self. c.getConfigJSON())

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

	# consistent amount of reconEvents (3 = 1primary+1noise+1decay)
        n_ev = len(spill_out['recon_events'])
        self.assertEqual(3, n_ev)
	# consistent amount of primary plane hits, charge and ToTs
        n_hits_0 = len(spill_out['recon_events'][0]['emr_event']\
				['emr_plane_hits'])
        self.assertEqual(24, n_hits_0)
        for i in range(0, n_hits_0):
            self.assertTrue(spill_out['recon_events'][0]['emr_event']\
				     ['emr_plane_hits'][i]['charge'] > 0)
            self.assertTrue(spill_out['recon_events'][0]['emr_event']\
				     ['emr_plane_hits'][i]['emr_bars'][0]\
				     ['emr_bar_hits'][0]['tot'] > 5)
	# consistent amount of noise and time from the primary
        n_hits_1 = len(spill_out['recon_events'][1]['emr_event']\
				['emr_plane_hits'])
        self.assertEqual(1, n_hits_1)
        self.assertTrue(spill_out['recon_events'][1]['emr_event']\
				 ['emr_plane_hits'][0]['emr_bars'][0]\
				 ['emr_bar_hits'][0]['delta_t'] - spill_out\
				 ['recon_events'][0]['emr_event']\
				 ['emr_plane_hits'][0]['emr_bars'][0]\
				 ['emr_bar_hits'][0]['delta_t'] > 20)

	# consistent amount of decay plane hits and bar hits in each plane 
        n_hits_2 = len(spill_out['recon_events'][2]['emr_event']\
				['emr_plane_hits'])
        self.assertEqual(8, n_hits_2)
        for i in range(0, n_hits_2):
            n_bars = len(spill_out['recon_events'][2]['emr_event']\
				  ['emr_plane_hits'][i]['emr_bars'])
            self.assertTrue(2, n_bars)

        # hits stored at the spill level (62 hits)
        self.assertTrue(spill_out['emr_spill_data']['emr_plane_hits'])
        spill_hits = 0
        for i in range(0, len(spill_out['emr_spill_data']\
				       ['emr_plane_hits'])):
            for j in range(0, len(spill_out['emr_spill_data']\
					   ['emr_plane_hits'][i]\
					   ['emr_bars'])):
                spill_hits += len(spill_out['emr_spill_data']\
					   ['emr_plane_hits'][i]\
					   ['emr_bars'][j]['emr_bar_hits'])
        self.assertEqual(62, spill_hits)

if __name__ == "__main__":
    unittest.main()

