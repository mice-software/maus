import json
import unittest
from Configuration import Configuration

from MapCppSimulation import MapCppSimulation

class MapCppSimulationTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.mapper = MapCppSimulation()
        success = self.mapper.Birth("""{"simulation_geometry_filename":"Stupid.dat"}""")
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

    def test_init(self):
         success = self.mapper.Birth("""{"simulation_geometry_filename":"Stupid.dat"}""")
         if success:
            raise Exception('InitializeFail', 'Could not start worker')

    def test_empty(self):
        result = self.mapper.Process("")
        doc = json.loads(result)
        print "doc\n",doc
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppSimulation" in doc["errors"])

    def test_no_mc_branch(self):
        result = self.mapper.Process("{}")
        doc = json.loads(result)
        print "doc\n",doc
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppSimulation" in doc["errors"])

    def test_mc_bad_type(self):
        result = self.mapper.Process("""{"mc" : 0.0}""")
        doc = json.loads(result)
        print "doc\n",doc
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppSimulation" in doc["errors"])

    @classmethod
    def tearDownClass(self):
        success = self.mapper.Death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
