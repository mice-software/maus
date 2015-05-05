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
# pylint: disable = C0103

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
        spill = maus_cpp.converter.json_repr(result)
        self.assertFalse("MapCppTOFSlabHits" in spill["errors"])
        # no slab hits in tof0 partEv0
        self.assertFalse('tof_0' in \
                        spill['recon_events'][0]['tof_event']['tof_slab_hits'])
        n_slab_hits_part_ev0_tof0 = \
             len(spill['recon_events'][0]['tof_event']['tof_slab_hits']['tof0'])
        self.assertEqual(n_slab_hits_part_ev0_tof0, 1)
        # test the calculation of the raw time
        raw_time_part_ev0_sl_hit0 = (416572-420998+1727593-1731992)/ \
                                                     2.*0.025
        raw_time_part_ev0_sl_hit0_rec = spill['recon_events'][0]['tof_event']\
                                        ['tof_slab_hits']['tof0'][0]['raw_time']
        self.assertEqual(raw_time_part_ev0_sl_hit0_rec,
                                                      raw_time_part_ev0_sl_hit0)
        # test the tof1 output
        n_slab_hits_part_ev0_tof1 = len(spill['recon_events'][0]\
                                      ['tof_event']['tof_slab_hits']['tof1'])
        self.assertEqual(n_slab_hits_part_ev0_tof1, 2)
        n_slab_hits_part_ev1_tof1 = len(spill['recon_events'][1]\
                                      ['tof_event']['tof_slab_hits']['tof1'])
        self.assertEqual(n_slab_hits_part_ev1_tof1, 1)
        # test the tof2 output - no slab hits in tof2 partEv1
        self.assertFalse(spill['recon_events'][1]\
                                      ['tof_event']['tof_slab_hits']['tof2'])

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that death works ok"""
        cls.mapper.death()
        cls.mapper = None

if __name__ == '__main__':
    unittest.main()
