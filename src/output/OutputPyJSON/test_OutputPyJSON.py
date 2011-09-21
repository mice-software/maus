import json
import unittest
import  tempfile
import gzip

from OutputPyJSON import OutputPyJSON

class OutputPyJSONTestCase(unittest.TestCase):
    """Test OutputPyJSON
    """

    @classmethod
    def setUpClass(self):
        """Prepare for test by setting up strings
        """
        self.test_string = """{"mc": [{"hits": [{"energy_deposited": 0.037781369897893102, "energy": 315.10944148506519, "volume_name": "Stage6.dat/TrackerSolenoid0.dat/Tracker0.dat/TrackerStation1.dat/TrackerViewW.datDoubletCores", "pid": 13, "track_id": 1, "channel_id": {"tracker_number": 0, "station_number": 1, "type": "Tracker", "plane_number": 2, "fiber_number": 107}, "charge": -1.0, "mass": 105.6584, "time": 1.2404951395988231}], "energy": 210, "unit_momentum": {"y": 0, "x": 0, "z": 1}, "particle_id": 13, "tracks": {"track1": {"parent_track_id": 1, "initial_momentum": {"y": -0.013085532209047269, "x": 0.045481821695065712, "z": 0.0023252953102620381}, "particle_id": 11, "track_id": 2, "final_position": {"y": -0.068452789962020408, "x": 0.1093100432810042, "z": -4912.1539662246769}, "initial_position": {"y": -0.1066636016684737, "x": 0.10807384560972209, "z": -4912.1943706252696}, "steps": [{"energy_deposited": 0.0021748808862488752, "position": {"y": -0.068452789962020408, "x": 0.1093100432810042, "z": -4912.1539662246769}, "momentum": {"y": -0.0, "x": -0.0, "z": 0.0}}], "final_momentum": {"y": -0.0, "x": -0.0, "z": 0.0}}}, "position": {"y": -0.10000000000000001, "x": 0.10000000000000001, "z": -5000}}]}\n"""

    def test_save_single_uncompressed(self):
        """Try saving one spill in uncompressed fashion
        """
        filename = tempfile.mkstemp()[1]
        output = OutputPyJSON(open(filename,'w'))

        self.assertTrue(output.birth("{}"))
        output.save(self.test_string)
        self.assertTrue(output.death())

        test_doc = json.loads(self.test_string)

        my_file = open(filename, 'r')
        read_doc = json.loads(my_file.readline())
        self.assertEqual(read_doc, test_doc)
        my_file.close()

    def test_save_single_compressed(self):
        """Try saving one spill using compressed gzip
        """
        filename = tempfile.mkstemp()[1]
        output = OutputPyJSON(gzip.GzipFile(filename, 'w'))

        self.assertTrue(output.birth("{}"))
        output.save(self.test_string)
        self.assertTrue(output.death())

        test_doc = json.loads(self.test_string)

        my_file = gzip.GzipFile(filename, 'r')
        read_doc = json.loads(my_file.readline())
        self.assertEqual(read_doc, test_doc)
        my_file.close()


if __name__ == '__main__':
    unittest.main()
