"""
Tests for ReducePyMatplotlibHistogram module.
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

from ReducePyMatplotlibHistogram import ReducePyMatplotlibHistogram

class ReducePyMatplotlibTester(ReducePyMatplotlibHistogram):
    """
    Simple ReducePyMatplotlibHistogram sub-class for testing.
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        ReducePyMatplotlibHistogram.__init__(self)
        self._histogram = None

    def _configure_at_birth(self, config_doc):
        """
        Configure worker from data cards. Overrides super-class method. 
        @param self Object reference.
        @param config_doc JSON document.
        @returns True if successful.
        """
        self._histogram = \
            ReducePyMatplotlibHistogram._create_histogram(self)
        return True

    def _update_histograms(self, spill):
        """
        Rescales histogram axes then creates image from it.
        @param self Object reference.
        @param spill Current spill.
        @returns list with histogram JSON document.
        @throws Exception if spill has an "error" key.
        """
        if spill.has_key("error"):
            raise Exception("error")
        ReducePyMatplotlibHistogram._rescale_axes( \
            self, self._histogram, 0, 1, 0, 1)
        image_doc = ReducePyMatplotlibHistogram._get_image_doc( \
            self, ["keywords"], "description", "test", self._histogram)
        return [image_doc]

    def _cleanup_at_death(self):
        """
        No sub-class-specific cleanup is done.
        @param self Object reference.
        @returns True
        """
        ReducePyMatplotlibHistogram._cleanup_at_death(self)
        return True

class ReducePyMatplotlibHistogramTestCase(unittest.TestCase): # pylint: disable=R0904, C0301
    """
    Test class for ReducePyMatplotlibHistogram.
    """

    @classmethod
    def setUpClass(self): # pylint: disable=C0202
        """ 
        Prepare for test by setting up worker.
        @param self Object reference.
        """
        self.__reducer = ReducePyMatplotlibTester()

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
        self.__reducer = ReducePyMatplotlibTester()
        success = self.__reducer.birth("""{"histogram_image_type":"png"}""")
        self.assertTrue(success, "reducer.birth() failed")
        self.assertEquals("png", self.__reducer.image_type, 
            "Unexpected reducer.image_type")

    def test_birth_bad_file_type(self):
        """
        Test configuration when "birth" is called with an unsupported
        file type. 
        @param self Object reference.
        """
        self.__reducer = ReducePyMatplotlibTester()
        with self.assertRaisesRegexp(ValueError,
            ".*Unsupported histogram image type.*"):
            self.__reducer.birth(
                """{"histogram_image_type":"no_such_extension"}""")

    def test_birth_auto_number(self):
        """
        Test configuration when birth is called with histogram
        auto-numbering enabled. 
        @param self Object reference.
        """
        self.__reducer = ReducePyMatplotlibTester()
        success = self.__reducer.birth("""{"histogram_auto_number": true}""")
        self.assertTrue(success, "reducer.birth() failed")
        self.assertTrue(self.__reducer.auto_number, 
            "Unexpected reducer.auto_number")

    def test_invalid_json(self):
        """
        Test "process" with a bad JSON document as an argument string.
        @param self Object reference.
        """
        result_str = self.__reducer.process("{")
        result = json.loads(result_str)
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        self.assertTrue("ReducePyMatplotlibTester" in errors,
            "No ReducePyMatplotlibTester field")        
        errors = errors["ReducePyMatplotlibTester"]
        self.assertTrue(len(errors) >= 1, "Missing error trace")
        self.assertEquals("<type 'exceptions.ValueError'>: Expecting object: line 1 column 0 (char 0)", errors[0], "Unexpected error trace") # pylint: disable=C0301

    def test_process_multiple_spills(self):
        """
        Test "process" with multiple JSON documents.
        @param self Object reference.
        """
        for i in range(0, 4):
            result = self.__process({})
            self.__check_result(i, result)

    def test_multiple_spills_auto_number(self):
        """
        Test "process" with multiple JSON documents and with 
        auto-numbering enabled.
        @param self Object reference.
        """
        self.__reducer = ReducePyMatplotlibTester()
        success = self.__reducer.birth("""{"histogram_auto_number": true}""")
        self.assertTrue(success, "reducer.birth() failed")
        self.assertTrue(self.__reducer.auto_number, 
            "Unexpected reducer.auto_number")
        for i in range(0, 4):
            result = self.__process({})
            self.__check_result(i, result)

    def test_error_spill(self):
        """
        Test "process" with a JSON document that causes an error to
        be raised.
        @param self Object reference.
        """
        json_doc = {"error": "error"}
        result = self.__process(json_doc)
        self.assertTrue("errors" in result, "No errors field")
        errors = result["errors"]
        self.assertTrue("ReducePyMatplotlibTester" in errors,
            "No ReducePyMatplotlibTester field")        
        errors = errors["ReducePyMatplotlibTester"]
        self.assertTrue(len(errors) >= 1, "Missing error trace")
        print errors[0]
        self.assertEquals("<type 'exceptions.Exception'>: error", errors[0], "Unexpected error trace") # pylint: disable=C0301

    def test_svg(self):
        """
        Test "process" can create SVG images.
        @param self Object reference.
        """
        self.__test_image_type("svg")

    def test_svgz(self):
        """
        Test "process" can create SVGZ images.
        @param self Object reference.
        """
        self.__test_image_type("svgz")

    def test_ps(self):
        """
        Test "process" can create PS images.
        @param self Object reference.
        """
        self.__test_image_type("ps")

    def test_rgba(self):
        """
        Test "process" can create RGBA images.
        @param self Object reference.
        """
        self.__test_image_type("rgba")

    def test_raw(self):
        """
        Test "process" can create RAW images.
        @param self Object reference.
        """
        self.__test_image_type("raw")

    def test_pdf(self):
        """
        Test "process" can create PDF images.
        @param self Object reference.
        """
        self.__test_image_type("pdf")

    def test_png(self):
        """
        Test "process" can create PNG images.
        @param self Object reference.
        """
        self.__test_image_type("png")

    def __test_image_type(self, image_type):
        """
        Test "process" can create images of the specified
        type. "birth" is called first to set up the image type.
        @param self Object reference.
        @param image_type Image type e.g. "eps".
        @returns JSON document string from "process".
        """
        self.__reducer = ReducePyMatplotlibTester()
        success = self.__reducer.birth(
            """{"histogram_image_type":"%s"}""" % image_type)
        self.assertTrue(success, "reducer.birth() failed")
        self.__process({})

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
        self.assertEquals(spill_id + 1, self.__reducer.spill_count,
            "Unexpected reducer.spill_count")
        self.assertTrue("image" in result, "No image field")
        image = result["image"]
        self.assertEquals(self.__reducer.image_type, image["image_type"],
            "Unexpected image_type")
        if (self.__reducer.auto_number):
            tag = "test%06d" % (spill_id + 1)
        else:
            tag = "test"
        self.assertEquals(tag, image["tag"], "Unexpected tag")
        self.assertTrue("keywords" in image, "No keywords field")
        self.assertTrue("description" in image, "No description field")
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
        if not success:
            raise Exception('Test setUp failed', 'reducer.death() failed')

    @classmethod
    def tearDownClass(self): # pylint: disable=C0202
        """ 
        Set the worker to None.
        @param self Object reference.
        """
        self.__reducer = None

if __name__ == '__main__':
    unittest.main()
