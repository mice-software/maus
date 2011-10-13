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
        self.assertTrue(result)
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
        # test withe some crazy events.
        result = self.mapper.process(data)
        spill = json.loads(result)
        # test the tof0 output
        n_part_events = len(spill['slab_hits']['tof0'])
        self.assertEqual(n_part_events,2)
        # test the calculation of the raw time
        raw_time_partEv0_slHit0 = (416572-420998+1727593-1731992)/2.*0.025
        n_slab_hits_partEv0_tof0 = len(spill['slab_hits']['tof0'][0])
        self.assertEqual(n_slab_hits_partEv0_tof0,1)
        no_slab_hits_in_partEv1_tof0 = True
        if spill['slab_hits']['tof0'][1]:
          no_slab_hits_in_partEv1_tof0 = False
        self.assertTrue(no_slab_hits_in_partEv1_tof0)
        raw_time_partEv0_slHit0_rec = spill['slab_hits']['tof0'][0][0]['raw_time']
        self.assertEqual(raw_time_partEv0_slHit0_rec,raw_time_partEv0_slHit0)
        # test the tof1 output
        n_part_events = len(spill['slab_hits']['tof1'])
        self.assertEqual(n_part_events,2)
        n_slab_hits_partEv0_tof1 = len(spill['slab_hits']['tof1'][0])
        self.assertEqual(n_slab_hits_partEv0_tof1,2)
        n_slab_hits_partEv1_tof1 = len(spill['slab_hits']['tof1'][1])
        self.assertEqual(n_slab_hits_partEv1_tof1,1)
        # test the tof2 output
        n_part_events = len(spill['slab_hits']['tof1'])
        self.assertEqual(n_part_events,2)
        no_slab_hits_in_partEv1_tof2 = True
        if spill['slab_hits']['tof2'][1]:
          no_slab_hits_in_partEv1_tof2 = False
        self.assertTrue(no_slab_hits_in_partEv1_tof2)

    @classmethod
    def tearDownClass(self):
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()