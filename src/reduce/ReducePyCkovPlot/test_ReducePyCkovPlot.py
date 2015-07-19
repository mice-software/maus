"""
Tests for ReducePyCkovPlot
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

from ReducePyCkovPlot import ReducePyCkovPlot

class ReducePyCkovPlotTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for ReducePyCkovPlot
    """

    @classmethod
    def setUpClass(self): # pylint: disable=C0202
        """ 
        Prepare for test by setting up worker.
        @param self Object reference.
        """
        self.__reducer = ReducePyCkovPlot()

    def setUp(self):
        """ 
        Invoke "birth" and check for success.
        @param self Object reference.
        """
        success = self.__reducer.birth("""{"root_batch_mode":1}""")
        if not success:
            raise Exception('Test setUp failed '+str(success),
                            'reducer.birth() failed')

    def test_birth_default(self):
        """
        Check default configuration after "birth" is called.
        @param self Object reference.
        """
        self.assertEquals(0, self.__reducer.spill_count, 
            "Unexpected reducer.spill_count")
        self.assertEquals("eps", self.__reducer.image_type, 
            "Unexpected reducer.image_type")
        self.assertTrue(not self.__reducer.auto_number, 
            "Unexpected reducer.auto_number")
    
    def test_birth_file_type(self):
        """
        Test configuration when "birth" is called with a supported
        file type. 
        @param self Object reference.
        """
        self.__reducer = ReducePyCkovPlot()
        success = self.__reducer.birth("""{"histogram_image_type":"png"}""")
        self.assertTrue(success, "reducer.birth() failed")
        self.assertEquals("png", self.__reducer.image_type, 
                          "Unexpected reducer.image_type")

    def test_invalid_json(self):
        """
        Test "process" with a bad JSON document as an argument string.
        @param self Object reference.
        """
        result_str = self.__reducer.process("{")
        result = json.loads(result_str)
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        self.assertTrue("ReducePyCkovPlot" in errors,
                        "No ReducePyCkovPlot field")        
        errors = errors["ReducePyCkovPlot"]
        self.assertTrue(len(errors) >= 1, "Missing error trace")
                
    def test_no_digits(self):
        """
        Test "process" with a JSON document with no "digits" entry.
        @param self Object reference.#
        """
        noDigit_json = {"run_number": 1, "maus_event_type": "Spill", "recon_events": [], "spill_number": 0, "errors": {}, "daq_event_type": "physics_event", "daq_data": {}} # pylint: disable=C0301
        result = self.__process(noDigit_json)
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        
        self.assertTrue("ReducePyCkovPlot" in errors,
                        "space points not in spill")        

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

    def __check_result(self, spill_id, result):
        """
        Validate results from "process". Check the current spill count
        in the worker and the image name is as expected. Then check
        the "image" contents. This method assumes the image type is
        "eps". 
        @param self Object reference.
        @param spill_id ID of spill just processed.
        @param result JSON document from "process".
        """
        json_doc = ('%s/src/reduce/ReducePyCkovPlot/processTest.txt' %
                    os.environ.get("MAUS_ROOT_DIR"))#pylint: disable=E0602

        result = self.__process(json_doc)
        self.assertEquals(spill_id + 1, self.__reducer.spill_count,
                          "Unexpected reducer.spill_count")

        self.assertTrue("image" in result, "No image field")
        image = result["image"]
        self.assertEquals(self.__reducer.image_type, image["image_type"],
                          "Unexpected image_type")
        if (self.__reducer.auto_number):
            tag = "CkovChargePMT1-8%06d" % (spill_id + 1)
        else:
            tag = "CkovChargePMT1-8"
            self.assertEquals(tag, image["tag"], "Unexpected tag")
            self.assertTrue("content" in image, "No content field")
            self.assertTrue("data" in image, "No data field")
            decoded_data = base64.b64decode(image["data"])
            self.assertTrue(decoded_data.find("EPS") != -1,
                            "Unexpected image data")
    def tearDown(self):
        """
        Invoke "death".
        @param self Object reference.
        """
        success = self.__reducer.death()
        if success != None:
            raise Exception('Test tearDown failed', 'reducer.death() failed')
        
if __name__ == '__main__':
    unittest.main()
