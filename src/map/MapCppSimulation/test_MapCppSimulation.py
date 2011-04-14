import json
import unittest
from Configuration import Configuration

from MapCppSimulation import MapCppSimulation

class MapCppSimulationTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.mapper = MapCppSimulation()
        success = self.mapper.Birth("""{"simulation_geometry_filename":"Stupid.dat"}""")
        self.particle = """
            {
                "position":{"x":1.,"y":2.,"z":3.},
                "momentum":{"x":4.,"y":5.,"z":6.},
                "particle_id":-13,
                "random_seed":1,
                "energy":110.,
                "time":7.
            }
        """
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

    @classmethod
    def tearDownClass(self):
        success = self.mapper.Death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

    ######## tests on Process #########
    def test_empty(self):
        result = self.mapper.Process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    def test_no_mc_branch(self): #mc should exist
        result = self.mapper.Process("{}")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppSimulation" in doc["errors"])

    def test_mc_bad_type(self): #mc should be a dict
        result = self.mapper.Process("""{"mc" : 0.0}""")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("MapCppSimulation" in doc["errors"])

    def test_mc_good(self): #should write some output
        good_event = """
        {
        "mc":["""
        good_event += self.particle+","+self.particle
        good_event += """
        ]
        }
        """
        print good_event
        result = self.mapper.Process(good_event)
        doc = json.loads(result)
        self.assertFalse("errors" in doc)
        print doc

if __name__ == '__main__':
    unittest.main()

