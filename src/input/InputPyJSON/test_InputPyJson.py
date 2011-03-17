import json
import unittest
import io

from InputPyJSON import InputPyJSON

class InputPyJSONTestCase(unittest.TestCase):
    """Tests InputPyJSON
    """
    
    @classmethod
    def setUpClass(self):
        """setup some test strings for all the tests
        """
        test_json = {}
        test_json['mc'] = []
        test_json['mc'].append({'tracks': {}})
        test_json['mc'][0]['tracks']['track1'] = {'particle_id' : 13}
        test_json['mc'][0]['tracks']['track1'] = {'particle_id' : -13}
        test_json['mc'][0]['tracks']['track1'] = {'particle_id' : 5}
        test_json['mc'][0]['hits'] = {}
        test_json['mc'].append(test_json['mc'][0])
        test_json['mc'].append({})
        self.test_string1 = json.dumps(test_json) + '\n'
        self.test_string1 = self.test_string1.encode("utf-8")

        # more like real documents
        self.test_string2 = u"""{"mc": [{"hits": [{"energy_deposited": 0.037781369897893102, "energy": 315.10944148506519, "volume_name": "Stage6.dat/TrackerSolenoid0.dat/Tracker0.dat/TrackerStation1.dat/TrackerViewW.datDoubletCores", "pid": 13, "track_id": 1, "channel_id": {"tracker_number": 0, "station_number": 1, "type": "Tracker", "plane_number": 2, "fiber_number": 107}, "charge": -1.0, "mass": 105.6584, "time": 1.2404951395988231}], "energy": 210, "unit_momentum": {"y": 0, "x": 0, "z": 1}, "particle_id": 13, "tracks": {"track1": {"parent_track_id": 1, "initial_momentum": {"y": -0.013085532209047269, "x": 0.045481821695065712, "z": 0.0023252953102620381}, "particle_id": 11, "track_id": 2, "final_position": {"y": -0.068452789962020408, "x": 0.1093100432810042, "z": -4912.1539662246769}, "initial_position": {"y": -0.1066636016684737, "x": 0.10807384560972209, "z": -4912.1943706252696}, "steps": [{"energy_deposited": 0.0021748808862488752, "position": {"y": -0.068452789962020408, "x": 0.1093100432810042, "z": -4912.1539662246769}, "momentum": {"y": -0.0, "x": -0.0, "z": 0.0}}], "final_momentum": {"y": -0.0, "x": -0.0, "z": 0.0}}}, "position": {"y": -0.10000000000000001, "x": 0.10000000000000001, "z": -5000}}]}\n"""

    def test_read100(self):
        """test read 100 events
        """
        my_file = io.StringIO(100*self.test_string2)
        my_input = InputPyJSON(my_file)
        test_doc = json.loads(self.test_string2)
        i = 0
        for doc in my_input.emitter():
            json_doc = json.loads(doc)
            self.assertTrue(json_doc == test_doc)
            i += 1
        self.assertEqual(i, 100)

    def test_bad(self):
        """test bad file content
        """
        my_file = io.StringIO(u"AFJKLAFJKALKF")
        my_input = InputPyJSON(my_file)
        with self.assertRaises(ValueError):
            next(my_input.emitter())

    def test_counter_lt(self):
        """test internal counter lt
        """
        my_file = io.StringIO(10*u"""{"test": 4}\n""")
        
        my_input = InputPyJSON(my_file, argNumberOfEvents=5)

        i = 0
        for doc in my_input.emitter():
            i += 1

        self.assertEqual(i, 5)

    def test_counter_gt(self):
        """test internal counter gt
        """
        my_file = io.StringIO(5*u"""{"test": 4}\n""")

        my_input = InputPyJSON(my_file, argNumberOfEvents=10)

        i = 0
        for doc in my_input.emitter():
            i += 1

        self.assertEqual(i, 5)

    def test_twice(self):
        """test trying to read twice gives nothing
        """
        my_file = io.StringIO(10*u"""{"test": 4}\n""")
        my_input = InputPyJSON(my_file, argNumberOfEvents=5)

        for doc in my_input.emitter():
            pass

        with self.assertRaises(StopIteration):
            next(my_input.emitter())
            



if __name__ == '__main__':
    unittest.main()
