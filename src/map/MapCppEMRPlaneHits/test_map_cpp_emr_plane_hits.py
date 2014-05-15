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
import json
import unittest
from Configuration import Configuration
import maus_cpp.converter
import MAUS

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
        n_ev = len(spill_out['recon_events'])
        #print spill_out["errors"]
        self.assertEqual(0, n_ev)
        self.assertFalse("MapCppEMRPlaneHits" in spill_out["errors"])

    def test_process(self):
        """Test MapCppEMRPlaneHits process method"""
        test2 = ('%s/src/map/MapCppEMRPlaneHits/processTest.json' % 
                 os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with some crazy events.
        result = self.mapper.process(data)
        #spill_in = json.loads(data)
        spill_out = maus_cpp.converter.json_repr(result)
        #print spill_out["errors"]
        self.assertFalse("MapCppEMRPlaneHits" in spill_out["errors"])
        n_ev = len(spill_out['recon_events'])
        self.assertEqual(3, n_ev)
        n_hits_0 = len(spill_out['recon_events'][0]['emr_event']\
                                ['emr_plane_hits'])
        self.assertEqual(1, n_hits_0)
        n_hits_1 = len(spill_out['recon_events'][1]['emr_event']\
                                ['emr_plane_hits'])
        self.assertEqual(2, n_hits_1)
        n_hits_2 = len(spill_out['recon_events'][2]['emr_event']\
                                ['emr_plane_hits'])
        self.assertEqual(1, n_hits_2)

if __name__ == "__main__":
    unittest.main()

