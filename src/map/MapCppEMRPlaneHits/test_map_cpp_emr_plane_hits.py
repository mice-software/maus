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

"""Tests for MapCppEMRPlaneHits"""

import os
import unittest
from Configuration import Configuration
import MAUS
import maus_cpp.converter

class TestMapCppEMRPlaneHits(unittest.TestCase): #pylint: disable=R0904
    """Tests for MapCppEMRPlaneHits"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """Sets a mapper and configuration"""
        cls.mapper = MAUS.MapCppEMRPlaneHits()
        cls.c = Configuration()

    def test_empty(self):
        """Check can handle empty configuration"""
        self.assertRaises(ValueError, self.mapper.birth, "")
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppEMRPlaneHits" in doc["errors"])

    def test_init(self):
        """Check birth with default configuration"""
        self.mapper.birth(self. c.getConfigJSON())

    def test_no_data(self):
        """Check that nothing happens in absence of data"""
        test1 = ('%s/src/map/MapCppEMRPlaneHits/noDataTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse("MapCppEMRPlaneHits" in spill_out["errors"])

        n_ev = len(spill_out['recon_events'])
        self.assertEqual(0, n_ev)

    def test_process(self):
        """Test MapCppEMRPlaneHits process method"""
        test2 = ('%s/src/map/MapCppEMRPlaneHits/processTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with some crazy events.
        result = self.mapper.process(data)
        spill_out = maus_cpp.converter.json_repr(result)
        self.assertFalse("MapCppEMRPlaneHits" in spill_out["errors"])

	# consistent amount of reconEvents (1DBB+fADC, 2 fADC only)
        n_ev = len(spill_out['recon_events'])
        self.assertEqual(3, n_ev)

	# consitent amount of plane hit in each event (1 per event)
        for i in range(0, n_ev):
            n_hits = len(spill_out['recon_events'][i]['emr_event']\
				  ['emr_plane_hits'])
            self.assertEqual(1, n_hits)

        # secondary hits stored at the spill level (1 triggerless hit)
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
        self.assertEqual(1, spill_hits)

if __name__ == "__main__":
    unittest.main()

