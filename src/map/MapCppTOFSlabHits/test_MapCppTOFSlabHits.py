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

import io  #  generic python library for I/O
import sys # for command line arguments
import os
import json
import unittest
from Configuration import Configuration
import MAUS

from MapCppTOFSlabHits import MapCppTOFSlabHits

class MapCppTOFSlabHitsTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.mapper = MAUS.MapCppTOFSlabHits()
        self.c = Configuration()

    def test_empty(self):
        result = self.mapper.birth("")
        self.assertFalse(result)
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_init(self):
        success = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(success)

    def test_no_data(self):
        test1 = '%s/src/map/MapCppTOFSlabHits/noDataTest.txt' %os.environ.get("MAUS_ROOT_DIR")
        f = open(test1,'r')
        data = f.read()
        # test with no data.
        result = self.mapper.process(data)
        spill = json.loads(result)
        no_slab_hits = True
        if 'slab_hits' in spill:
         no_slab_hits = False
        self.assertTrue(no_slab_hits)

    def test_process(self):
        test2 = '%s/src/map/MapCppTOFSlabHits/processTest.txt' %os.environ.get("MAUS_ROOT_DIR")
        f = open(test2,'r')
        data = f.read()
        # test with some crazy events.
        result = self.mapper.process(data)
        spill = json.loads(result)
        # test the tof0 output
        n_part_events = len(spill['slab_hits']['tof0'])
        self.assertEqual(n_part_events,3)
        self.assertFalse(spill['slab_hits']['tof0'][1]) # no slab hits in tof0 partEv0
        n_slab_hits_partEv0_tof0 = len(spill['slab_hits']['tof0'][0])
        self.assertEqual(n_slab_hits_partEv0_tof0,1)
        # test the calculation of the raw time
        raw_time_partEv0_slHit0 = (416572-420998+1727593-1731992)/2.*0.025
        raw_time_partEv0_slHit0_rec = spill['slab_hits']['tof0'][0][0]['raw_time']
        self.assertEqual(raw_time_partEv0_slHit0_rec,raw_time_partEv0_slHit0)
        # test the tof1 output
        n_part_events = len(spill['slab_hits']['tof1'])
        self.assertEqual(n_part_events,3)
        n_slab_hits_partEv0_tof1 = len(spill['slab_hits']['tof1'][0])
        self.assertEqual(n_slab_hits_partEv0_tof1,2) 
        n_slab_hits_partEv1_tof1 = len(spill['slab_hits']['tof1'][1])
        self.assertEqual(n_slab_hits_partEv1_tof1,1)
        # test the tof2 output
        n_part_events = len(spill['slab_hits']['tof1'])
        self.assertEqual(n_part_events,3)
        self.assertFalse(spill['slab_hits']['tof2'][1]) # no slab hits in tof2 partEv1

    @classmethod
    def tearDownClass(self):
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()