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
        success = self.mapper.birth(self. c.getConfigJSON())
        self.assertTrue(success)
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    @classmethod
    def tearDownClass(self):
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()