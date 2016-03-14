# This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
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

"""Tests for MapCppTOFSlabHits"""
# pylint: disable = C0103, C0301

import os
import unittest
from Configuration import Configuration
import maus_cpp.converter
import MAUS

class MapCppTOFSlabHitsTestCase(unittest.TestCase):# pylint: disable = R0904
    """Tests for MapCppTOFSlabHits"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """init a TOFSlabHits object"""
        cls.mapper = MAUS.MapCppTOFSlabHits()
        cls.c = Configuration()

    #def test_empty(self):
        #"""Check against configuration is empty"""
        #self.assertRaises(ValueError, self.mapper.birth, "")
        #result = self.mapper.process("")
        #doc = maus_cpp.converter.json_repr(result)
        #self.assertTrue("errors" in doc)
        #self.assertTrue("MapCppTOFSlabHits" in doc["errors"])

    def test_init(self):
        """Check that birth works properly"""
        self.mapper.birth(self. c.getConfigJSON())

    def test_no_data(self):
        """Check that against data stream is empty"""
        test1 = ('%s/src/map/MapCppTOFSlabHits/noDataTest.txt' %
                                                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill = maus_cpp.converter.json_repr(result)
        no_slab_hits = True
        if 'slab_hits' in spill:
            no_slab_hits = False
        self.assertTrue(no_slab_hits)

    def test_process(self):
        """Check MapCppTOFSlabHits process function"""
        test2 = ('%s/src/map/MapCppTOFSlabHits/processTest.json' %
                                                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with some crazy events.
        result = self.mapper.process(data)
        # get recon event 0
        rec0 = result.GetSpill().GetReconEvents()[0]
        # get recon event 1
        rec1 = result.GetSpill().GetReconEvents()[1]
        # get # tof0 slab hits for event 0 - should be = 1
        n_slab_hits_part_ev0_tof0 = rec0.GetTOFEvent().GetTOFEventSlabHit().GetTOF0SlabHitArraySize()
        self.assertEqual(n_slab_hits_part_ev0_tof0, 1)
        # test the calculation of the raw time
        raw_time_part_ev0_sl_hit0 = (416572-420998+1727593-1731992)/ \
                                                     2.*0.025
        raw_time_part_ev0_sl_hit0_rec = rec0.GetTOFEvent().GetTOFEventSlabHit().GetTOF0SlabHitArrayElement(0).GetRawTime()
        self.assertEqual(raw_time_part_ev0_sl_hit0_rec,
                                                      raw_time_part_ev0_sl_hit0)
        # test the tof1 output
        # check the number of slab hits for events 0 and event 1
        n_slab_hits_part_ev0_tof1 = rec0.GetTOFEvent().GetTOFEventSlabHit().GetTOF1SlabHitArraySize()
        n_slab_hits_part_ev1_tof1 = rec1.GetTOFEvent().GetTOFEventSlabHit().GetTOF1SlabHitArraySize()
        self.assertEqual(n_slab_hits_part_ev0_tof1, 2)
        self.assertEqual(n_slab_hits_part_ev1_tof1, 1)
        # test the tof2 output - no slab hits in tof2 partEv1
        n_slab_hits_part_ev1_tof2 = rec1.GetTOFEvent().GetTOFEventSlabHit().GetTOF2SlabHitArraySize()
        self.assertEqual(n_slab_hits_part_ev1_tof2, 0)

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that death works ok"""
        cls.mapper.death()
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
