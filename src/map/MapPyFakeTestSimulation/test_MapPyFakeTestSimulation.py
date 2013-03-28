#pylint: disable =C0103,R0904
"""test_MapPyFakeTestSimulation.py"""
# import hashlib
# import json
import unittest
# import io
# from Configuration import Configuration

from MapPyFakeTestSimulation import MapPyFakeTestSimulation

class MapPyFakeTestSimulationTestCase(unittest.TestCase):
    """ Testing MapPyFakeTestSimulation """
    @classmethod
    def setUpClass(self):#pylint: disable =C0103,C0202
        """ Set Up """
        self.expectedShaSumTracks =   '9ffe5bf5173024f6f02fbce036b6313a36970b53'

    def test_sha_tracks(self):
        """ a test """
        mapper = MapPyFakeTestSimulation()
        success = mapper.birth("{}")
        self.assertTrue(success)
        # result = mapper.process("")
        # shaSum = hashlib.sha1(result).hexdigest()
        # self.assertEqual(shaSum, self.expectedShaSumTracks)
        mapper.death()
        

if __name__ == '__main__':
    unittest.main()
