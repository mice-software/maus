import hashlib
import json
import unittest
import io
from Configuration import Configuration

from MapPyFakeTestSimulation import MapPyFakeTestSimulation

class MapPyFakeTestSimulationTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.expectedShaSumTracks =   '9ffe5bf5173024f6f02fbce036b6313a36970b53'

    def test_sha_tracks(self):
        mapper = MapPyFakeTestSimulation()
        success = mapper.Birth("{}")
        self.assertTrue(success)
        result = mapper.Process("")
        shaSum = hashlib.sha1(result).hexdigest()
        self.assertEqual(shaSum, self.expectedShaSumTracks)
        mapper.Death()
        

if __name__ == '__main__':
    unittest.main()
