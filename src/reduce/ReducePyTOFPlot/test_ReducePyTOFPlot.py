"""
Tests for ReducePyTOFPlot module.
"""
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

# pylint: disable=C0103

import base64
import json
import unittest
import sys
sys.argv.append('-b')
import glob
import os
from ReducePyTOFPlot import ReducePyTOFPlot

class ReducePyTOFPlotTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for ReducePyTOFPlot.
    """

    @classmethod
    def setUpClass(self): # pylint: disable=C0202
        """ 
        Prepare for test by setting up worker.
        @param self Object reference.
        """
        self.__reducer = ReducePyTOFPlot()

    def setUp(self):
        """ 
        Invoke "birth" and check for success.
        @param self Object reference.
        """
        success = self.__reducer.birth("{}")
        if not success:
            raise Exception('Test setUp failed', 'reducer.birth() failed')

    def test_birth_default(self):
        """
        Check default configuration after "birth" is called.
        @param self Object reference.
        """
        self.assertEquals(0, self.__reducer.spill_count,
            "Unexpected reducer.spill_count")
        self.assertEquals("eps", self.__reducer.filetype,
            "Unexpected reducer.filetype")
        self.assertEquals(1, self.__reducer.root_batch_mode,
            "Unexpected reducer.root_batch_mode")

    def test_birth_file_type(self):
        """
        Test configuration when "birth" is called with a supported
        file type. 
        @param self Object reference.
        """

        self.__reducer = ReducePyTOFPlot()
        success = self.__reducer.birth("""{"filetype":"eps"}""")
        self.assertTrue(success, "reducer.birth() failed")
        self.assertEquals("eps", self.__reducer.filetype, 
            "Unexpected reducer.filetype")

    def test_invalid_json(self):
        """
        Test "process" with a bad JSON document as an argument string.
        @param self Object reference.
        """

        result_str = self.__reducer.process("{")
        result = json.loads(result_str)
        self.assertTrue("errors" in result, "No errors field")
        self.assertTrue("bad_json_document" in result["errors"],
            "No bad_json_document field")        

    def test_no_slab_hits(self):
        """
        Test "process" with a JSON document with no "digits" entry.
        @param self Object reference.
        """

        result = self.__process({})
        self.assertTrue("errors" in result, "No errors field")
        self.assertTrue("no_slab_hits" in result["errors"],
            "No no_slab_hits field")
    

    def test_empty_slab_hits(self):
        """
        Test "process" with a JSON document with an empty "digits"
        entry. 
        @param self Object reference.
        """

        json_doc = {"slab_hits":{}}
        result = self.__process(json_doc)
        self.assertTrue("errors" in result, "No errors field")
        self.assertTrue("no_space_points" in result["errors"],
            "No no_space_points field")

    def test_empty_space_points(self):
        """
        Test "process" with a JSON document with an empty "digits"
        entry. 
        @param self Object reference.
        """

        json_doc = {"space_points":{}}
        result = self.__process(json_doc)
        self.assertTrue("errors" in result, "No errors field")
        self.assertTrue("no_slab_hits" in result["errors"],
            "No no_slab_hits field")

    def test_empty_digits(self):
        """
        Test "process" with a JSON document with an empty "digits"
        entry. 
        @param self Object reference.
        """

        json_doc = {"slab_hits":{}, "space_points":{}}
        result = self.__process(json_doc)
        self.assertTrue("errors" in result, "No errors field")
        self.assertTrue("no_space_points" in result["errors"],
            "No no_space_points field")


    def test_digits(self):
        """
        Test "process" with a JSON document with an empty "digits"
        entry. 
        @param self Object reference.
        """

        json_doc = {"slab_hits": 
                   { "tof1": [[{"slab": 4, "plane": 0, "pmt1": {}, "pmt0": {}},
                   { "slab": 3, "plane": 1, "pmt1": { }, "pmt0": {}}]],
                   "tof2": [ [ { "slab": 4, "plane": 0, "pmt1": {}, "pmt0": {}},
                   { "slab": 3, "plane": 1, "pmt1": { }, "pmt0": {}}]],
                   "tof0": [ [ { "slab": 4, "plane": 0, "pmt1": {}, "pmt0": {}},
                   { "slab": 3, "plane": 1, "pmt1": { }, "pmt0": {}}]]
	          },
                  "space_points": 
                  { "tof1": [ [ { "slabY": 3, "slabX": 4, "time": 0.06 } ]],
                    "tof0": [ [ { "slabY": 3, "slabX": 4, "time": 0.03 } ]],
                    "tof2": [ [ { "slabY": 3, "slabX": 4, "time": 0.05 } ]],
	          }
                  }
        result = self.__process(json_doc)
        print result
        self.assertTrue("image" in result, "No image field")
        image = result["image"]
        self.assertEquals(self.__reducer.filetype, image["image_type"],
            "Unexpected image_type")
        tag_count = self.__reducer.spill_count
        tag = "tof_times_%06d" % tag_count
        self.assertEquals(tag, image["tag"], "Unexpected tag")
        self.assertTrue("content" in image, "No content field")
        self.assertTrue("data" in image, "No data field")
        decoded_data = base64.b64decode(image["data"])
        self.assertTrue(decoded_data.find("EPS") != -1,
            "Unexpected image data")


    def __process(self, json_doc):
        """
        Convert given JSON document to a string and pass to "process".
        @param self Object reference.
        @param json_doc JSON document.
        @returns JSON document string from "process".
        """

        json_str = json.dumps(json_doc)
        result_str = self.__reducer.process(json_str)
        return json.loads(result_str)


    def tearDown(self):
        """
        Invoke "death".
        @param self Object reference.
        """

        success = self.__reducer.death()
        if not success:
            raise Exception('Test setUp failed', 'reducer.death() failed')
        for filename in glob.glob('tof_*.eps') :
            os.remove(filename)	

    @classmethod
    def tearDownClass(self): # pylint: disable=C0202
        """ 
        Set the worker to None.
        @param self Object reference.
        """
        self.__reducer = None

if __name__ == '__main__':
    unittest.main()
