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

"""Tests for MapCppKLCellHits"""
# pylint: disable = C0103

import os
import unittest
from Configuration import Configuration
import maus_cpp.converter
import MAUS

class MapCppKLCellHitsTestCase(unittest.TestCase):# pylint: disable = R0904
    """Tests for MapCppKLCellHits"""
    @classmethod
    def setUpClass(cls): # pylint: disable = C0103
        """init a KLCellHits object"""
        cls.mapper = MAUS.MapCppKLCellHits()
        cls.c = Configuration()

    def test_empty(self):
        """Check against configuration is empty"""
        self.assertRaises(ValueError, self.mapper.birth, "")
        result = self.mapper.process("")
        doc = maus_cpp.converter.json_repr(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppKLCellHits" in doc["errors"])

    def test_init(self):
        """Check that birth works properly"""
        self.mapper.birth(self. c.getConfigJSON())

    def test_no_data(self):
        """Check that against data stream is empty"""
        test1 = ('%s/src/map/MapCppKLCellHits/noDataTest.txt' %
                                                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test1,'r')
        data = fin.read()
        # test with no data.
        result = self.mapper.process(data)
        spill = maus_cpp.converter.json_repr(result)
        no_cell_hits = True
        if 'cell_hits' in spill:
            no_cell_hits = False
        self.assertTrue(no_cell_hits)

    def test_process(self):
        """Check MapCppKLCellHits process function"""
        test2 = ('%s/src/map/MapCppKLCellHits/kl_cellhit_test.txt' %
                                                os.environ.get("MAUS_ROOT_DIR"))
        fin = open(test2,'r')
        data = fin.read()
        # test with some crazy events.
        result = self.mapper.process(data)
        spill = maus_cpp.converter.json_repr(result)
        self.assertFalse("MapCppKLCellHits" in spill["errors"])

        # test the kl output
        n_cell_hits_part_ev0_kl = \
             len(spill['recon_events'][0]['kl_event']['kl_cell_hits']['kl'])
        self.assertEqual(n_cell_hits_part_ev0_kl, 2)  
        n_cell_hits_part_ev1_kl = len(spill['recon_events'][1]\
                                      ['kl_event']['kl_cell_hits']['kl'])
        self.assertEqual(n_cell_hits_part_ev1_kl, 1)

        product_part_ev1_cell_hit = 2 * 1078 * 1274 / (1078 + 1274)
        product_part_ev1_cell_hit_rec = spill['recon_events'][1]['kl_event']\
                                     ['kl_cell_hits']['kl'][0]['charge_product']
        self.assertEqual(product_part_ev1_cell_hit_rec,
                                                      product_part_ev1_cell_hit)
        # no cell hits in event 2
        self.assertFalse(spill['recon_events'][2]\
                                      ['kl_event']['kl_cell_hits']['kl'])

    @classmethod
    def tearDownClass(cls): # pylint: disable = C0103
        """Check that death works ok"""
        cls.mapper.death()

if __name__ == '__main__':
    unittest.main()
