#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
#
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.

#pylint: disable = C0103

"""Tests for OutputPyJSON module
"""

import os
import json
import unittest
import gzip

from OutputPyJSON import OutputPyJSON

class OutputPyJSONTestCase(unittest.TestCase): #pylint: disable = R0904
    """Test OutputPyJSON
    """

    @classmethod
    def setUpClass(self): #pylint: disable = C0103, C0202
        """Prepare for test by setting up strings
        """
        self.test_string = json.dumps({"mc": [
          {"hits": [{"energy_deposited": 0.037781369897893102, "energy": 315.10944148506519, "volume_name": "Stage6.dat/TrackerSolenoid0.dat/Tracker0.dat/TrackerStation1.dat/TrackerViewW.datDoubletCores", "pid": 13, "track_id": 1, "channel_id": {"tracker_number": 0, "station_number": 1, "type": "Tracker", "plane_number": 2, "fiber_number": 107}, "charge": -1.0, "mass": 105.6584, "time": 1.2404951395988231}], "energy": 210, "unit_momentum": {"y": 0, "x": 0, "z": 1}, "particle_id": 13, "tracks": {"track1": {"parent_track_id": 1, "initial_momentum": {"y": -0.013085532209047269, "x": 0.045481821695065712, "z": 0.0023252953102620381}, "particle_id": 11, "track_id": 2, "final_position": {"y": -0.068452789962020408, "x": 0.1093100432810042, "z": -4912.1539662246769}, "initial_position": {"y": -0.1066636016684737, "x": 0.10807384560972209, "z": -4912.1943706252696}, "steps": [{"energy_deposited": 0.0021748808862488752, "position": {"y": -0.068452789962020408, "x": 0.1093100432810042, "z": -4912.1539662246769}, "momentum": {"y": -0.0, "x": -0.0, "z": 0.0}}], "final_momentum": {"y": -0.0, "x": -0.0, "z": 0.0}}}, "position": {"y": -0.10000000000000001, "x": 0.10000000000000001, "z": -5000}}]}) #pylint: disable = C0301

    def test_save_single_uncompressed(self):
        """Try saving one spill in uncompressed fashion
        """
        output = OutputPyJSON(open(self.tmp_file,'w'))

        self.assertTrue(output.birth("{}"))
        self.assertTrue(output.save(self.test_string))
        self.assertTrue(output.death())
        self.assertFalse(output.save(self.test_string)) # attempting to write to
                                                        # closed file
        test_doc = json.loads(self.test_string)

        my_file = open(self.tmp_file, 'r')
        read_doc = json.loads(my_file.readline())
        self.assertEqual(read_doc, test_doc)
        my_file.close()

    def test_save_single_compressed(self):
        """Try saving one spill using compressed gzip
        """
        output = OutputPyJSON(gzip.GzipFile(self.tmp_file, 'w'))

        self.assertTrue(output.birth("{}"))
        output.save(self.test_string)
        self.assertTrue(output.death())
        self.assertFalse(output.save(self.test_string)) # attempting to write to
                                                        # closed file

        test_doc = json.loads(self.test_string)

        my_file = gzip.GzipFile(self.tmp_file, 'r')
        read_doc = json.loads(my_file.readline())
        self.assertEqual(read_doc, test_doc)
        my_file.close()

    def test_birth_text_format(self):
        """Test OutputPyJson loads a text file from datacards correctly."""
        output = OutputPyJSON()
        json_doc = json.dumps({"output_json_file_name":self.tmp_file,
                               "output_json_file_type":"text"})
        output.birth(json_doc)
        self.assertTrue(output.save(self.test_string))
        self.assertTrue(output.death())

        my_file = open(self.tmp_file, 'r')
        read_doc = json.loads(my_file.readline())
        self.assertEqual(read_doc, json.loads(self.test_string))
        my_file.close()

    def test_birth_gzip_format(self):
        """Test OutputPyJson loads a gzip file from datacards correctly."""
        output = OutputPyJSON()
        json_doc = json.dumps({"output_json_file_name":self.tmp_file,
                               "output_json_file_type":"gzip"})
        output.birth(json_doc)
        self.assertTrue(output.save(self.test_string))
        self.assertTrue(output.death())

        my_file = gzip.GzipFile(self.tmp_file, 'r')
        line = my_file.readline()
        read_doc = json.loads(line)
        self.assertEqual(read_doc, json.loads(self.test_string))
        my_file.close()


    def test_death(self):
        """Test OutputPyJson handles exceptions on bad death()."""
        output = OutputPyJSON(gzip.GzipFile(self.tmp_file, 'w'))

        self.assertTrue(output.birth("{}"))
        self.assertTrue(output.death())
        self.assertTrue(output.death()) #close() on closed is a no-op

    tmp_file = os.path.join(os.environ["MAUS_ROOT_DIR"], "tmp", \
                            "OutputPyJSON.tmp")

if __name__ == '__main__':
    unittest.main()
