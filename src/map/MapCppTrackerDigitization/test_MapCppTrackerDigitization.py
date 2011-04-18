import json
import unittest
from Configuration import Configuration

from MapCppTrackerDigitization import MapCppTrackerDigitization
from MapPyFakeTestSimulation import MapPyFakeTestSimulation

class MapCppTrackerDigitizationTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.mapper = MapCppTrackerDigitization()
        self.preMapper = MapPyFakeTestSimulation()
        c = Configuration()
        self.preMapper.birth(c.getConfigJSON())

        success = self.mapper.birth(c.getConfigJSON())
        
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')


    def test_empty(self):
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_no_mc_branch(self):
        result = self.mapper.process("{}")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("missing_branch" in doc["errors"])

    def test_mc_bad_type(self):
        result = self.mapper.process("""{"mc" : 0.0}""")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_type" in doc["errors"])

    def test_digit(self):
        data = self.preMapper.process("{}")
        newdata = self.mapper.process(data)

        self.assertTrue("digits" in json.loads(newdata))

    @classmethod
    def tearDownClass(self):
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
