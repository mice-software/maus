import json
import unittest
from Configuration import Configuration

from MapCppBeamMaker import MapCppBeamMaker

class MapCppBeamMakerTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.mapper = MapCppBeamMaker()
        c = Configuration()
        success = self.mapper.Birth(c.getConfigJSON())
        
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')


    def test_empty(self):
        result = self.mapper.Process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)

    def test_no_mc_branch(self):
        result = self.mapper.Process("{}")
        doc = json.loads(result)
        self.assertFalse("errors" in doc)

    def test_mc_exists(self):
        result = self.mapper.Process("""{"mc" : 0.0}""")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)

    @classmethod
    def tearDownClass(self):
        success = self.mapper.Death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
