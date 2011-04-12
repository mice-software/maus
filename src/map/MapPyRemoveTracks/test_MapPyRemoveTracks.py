import json
import unittest

from Configuration import Configuration
import io
from MapPyRemoveTracks import MapPyRemoveTracks

class MapPyRemoveTracksTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        testJson = {}
        testJson['mc'] = []
        testJson['mc'].append({'tracks': {}})
        testJson['mc'][0]['tracks']['track1'] = {'particle_id' : 13}
        testJson['mc'][0]['tracks']['track2'] = {'particle_id' : -13}
        testJson['mc'][0]['tracks']['track3'] = {'particle_id' : 5}
        testJson['mc'][0]['hits'] = {}
        testJson['mc'].append(testJson['mc'][0])
        #testJson['mc'].append({})
        self.testString1 = json.dumps(testJson)

        # more like real documents
        self.testString2 = """{"mc": [{"hits": [{"energy_deposited": 0.037781369897893102, "energy": 315.10944148506519, "volume_name": "Stage6.dat/TrackerSolenoid0.dat/Tracker0.dat/TrackerStation1.dat/TrackerViewW.datDoubletCores", "pid": 13, "track_id": 1, "channel_id": {"tracker_number": 0, "station_number": 1, "type": "Tracker", "plane_number": 2, "fiber_number": 107}, "charge": -1.0, "mass": 105.6584, "time": 1.2404951395988231}], "energy": 210, "unit_momentum": {"y": 0, "x": 0, "z": 1}, "particle_id": 13, "tracks": {"track1": {"parent_track_id": 1, "initial_momentum": {"y": -0.013085532209047269, "x": 0.045481821695065712, "z": 0.0023252953102620381}, "particle_id": 11, "track_id": 2, "final_position": {"y": -0.068452789962020408, "x": 0.1093100432810042, "z": -4912.1539662246769}, "initial_position": {"y": -0.1066636016684737, "x": 0.10807384560972209, "z": -4912.1943706252696}, "steps": [{"energy_deposited": 0.0021748808862488752, "position": {"y": -0.068452789962020408, "x": 0.1093100432810042, "z": -4912.1539662246769}, "momentum": {"y": -0.0, "x": -0.0, "z": 0.0}}], "final_momentum": {"y": -0.0, "x": -0.0, "z": 0.0}}}, "position": {"y": -0.10000000000000001, "x": 0.10000000000000001, "z": -5000}}]}"""

    def test_empty(self):
        mapper = MapPyRemoveTracks()

        c = Configuration()
        success = mapper.birth(c.getConfigJSON())
        self.assertTrue(success)

        result = mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

        mapper.death()

    def test_no_mc_branch(self):
        mapper = MapPyRemoveTracks()

        c = Configuration()
        success = mapper.birth(c.getConfigJSON())
        self.assertTrue(success)

        result = mapper.process("{}")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("missing_branch" in doc["errors"])

        mapper.death()

    def test_removes_tracks(self):
        mapper = MapPyRemoveTracks()

        c = Configuration()
        myConfig = io.StringIO(u"keep_only_muon_tracks = False")

        success = mapper.birth(c.getConfigJSON(myConfig))
        self.assertTrue(success)

        for testString in [self.testString1, self.testString2]:
            result = mapper.process(testString)
            doc = json.loads(result)
            self.assertFalse("errors" in doc)
            for particle in doc["mc"]:
                self.assertFalse("tracks" in particle)

        mapper.death()

    def test_remove_non_muon(self):
        mapper = MapPyRemoveTracks()

        c = Configuration()
        myConfig = io.StringIO(u"keep_only_muon_tracks = True")

        success = mapper.birth(c.getConfigJSON(myConfig))

        self.assertTrue(success)

        found = False
        result = mapper.process(self.testString1)
        doc = json.loads(result)
        self.assertFalse("errors" in doc)
        for particle in doc["mc"]:
            self.assertTrue("tracks" in particle)
            
            for track, value in list(particle['tracks'].items()):
                self.assertTrue(abs(value['particle_id']) == 13)
                found = True

        self.assertTrue(found)

        mapper.death()



if __name__ == '__main__':
    unittest.main()
