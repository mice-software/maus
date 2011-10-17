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
        self.__reducer = ReducePyMatplotlibHistogram()

    def setUp(self):
        """ 
        Invoke worker's birth method and check for success.
        @param self Object reference.
        """
        success = self.__reducer.birth("{}")
        if not success:
            raise Exception('Test setUp failed', 'reducer.birth() failed')

    def test_birth_default(self):
        """
        Test worker's default values after birth is called.
        @param self Object reference.
        """
        self.assertEquals(0, self.__reducer.spill_count, 
            "Unexpected reducer.spill_count")
        self.assertEquals("eps", self.__reducer.image_type, 
            "Unexpected reducer.image_type")

    def test_birth_file_type(self):
        """
        Test worker when birth is called with supported file type.
        @param self Object reference.
        """
        self.__reducer = ReducePyMatplotlibHistogram()
        success = self.__reducer.birth("""{"histogram_image_type":"png"}""")
        self.assertTrue(success, "reducer.birth() failed")
        self.assertEquals("png", self.__reducer.image_type, 
            "Unexpected reducer.image_type")

    def test_birth_bad_file_type(self):
        """
        Test worker when birth is called with unsupported file type.
        @param self Object reference.
        """
        self.__reducer = ReducePyMatplotlibHistogram()
        with self.assertRaisesRegexp(ValueError,
            ".*Unsupported histogram image type.*"):
            self.__reducer.birth(
                """{"histogram_image_type":"no_such_extension"}""")

    def test_invalid_json(self):
        """
        Test worker's process method with a bad JSON document
        as an argument string.
        @param self Object reference.
        """
        result_str = self.__reducer.process("{")
        result = json.loads(result_str)
        self.assertTrue("errors" in result, "No errors field")
        self.assertTrue("bad_json_document" in result["errors"],
            "No bad_json_document field")        

    def test_no_digits(self):
        """
        Test worker's process method with a JSON document with no 
        digits entry.
        @param self Object reference.
        """
        result = self.__process({})
        self.assertTrue("errors" in result, "No errors field")
        self.assertTrue("no_digits" in result["errors"],
            "No no_digits field")        

    def test_empty_digits(self):
        """
        Test worker's process method with a JSON document with an
        empty digits entry.
        @param self Object reference.
        """
        json_doc = {"digits":{}}
        result = self.__process(json_doc)
        self.__check_result(0, result)

    def test_no_channel_id(self):
        """
        Test worker's process method with a JSON document with a
        digits entry but no channel ID.
        @param self Object reference.
        """
        json_doc = {"digits": {"ac_counts":22, "tc_counts":22}}
        result = self.__process(json_doc)
        self.__check_result(0, result)

    def test_no_tracker(self):
        """
        Test worker's process method with a JSON document with a
        digits entry and channel ID but no tracker.
        @param self Object reference.
        """
        json_doc = {"digits":[{"adc_counts":1, 
            "channel_id":{}, "tdc_counts":1}]}
        result = self.__process(json_doc)
        self.__check_result(0, result)

    def test_no_matching_tracker(self):
        """
        Test worker's process method with a JSON document with a
        digits entry and channel ID but no matching tracker.
        @param self Object reference.
        """
        json_doc = {"digits":[{"adc_counts":1,
             "channel_id":{"type":"UnknownTracker"}, "tdc_counts":1}]}
        result = self.__process(json_doc)
        self.__check_result(0, result)

    def test_adc_only(self):
        """
        Test worker's process method with a JSON document with a
        digits entry and channel ID and matching tracker and
        ADC counts only.
        @param self Object reference.
        """
        json_doc = {"digits":[{"adc_counts":1, 
            "channel_id":{"type":"Tracker"}}]}
        result = self.__process(json_doc)
        self.__check_result(0, result)

    def test_tdc_only(self):
        """
        Test worker's process method with a JSON document with a
        digits entry and channel ID and matching tracker and
        TDC counts only.
        @param self Object reference.
        """
        json_doc = {"digits": [{"tdc_counts":1, 
            "channel_id":{"type":"Tracker"}}]}
        result = self.__process(json_doc)
        self.__check_result(0, result)

    def test_multiple_spills_digits(self):
        """
        Test worker's process method with a JSON document with 
        multiple digits entries in multiple JSON documents.
        @param self Object reference.
        """
        for i in range(0, 4):
            json_doc = {"digits":[]}
            for j in range(0, 10):
                json_doc["digits"].append(self.__get_digit(j, j))
            result = self.__process(json_doc)
            self.__check_result(i, result)

    def test_svg(self):
        """
        Test worker's process method can create SVG images.
        @param self Object reference.
        """
        self.__test_image_type("svg")

    def test_svgz(self):
        """
        Test worker's process method can create SVGZ images.
        @param self Object reference.
        """
        self.__test_image_type("svgz")

    def test_ps(self):
        """
        Test worker's process method can create PS images.
        @param self Object reference.
        """
        self.__test_image_type("ps")

    def test_rgba(self):
        """
        Test worker's process method can create RGBA images.
        @param self Object reference.
        """
        self.__test_image_type("rgba")

    def test_raw(self):
        """
        Test worker's process method can create RAW images.
        @param self Object reference.
        """
        self.__test_image_type("raw")

    def test_pdf(self):
        """
        Test worker's process method can create PDF images.
        @param self Object reference.
        """
        self.__test_image_type("pdf")

    def test_png(self):
        """
        Test worker's process method can create PNG images.
        @param self Object reference.
        """
        self.__test_image_type("png")

    def __get_digit(self, adc, tdc): #pylint: disable=R0201
        """
        Get a digits entry with the given ADC and TDC counts.
        @param self Object reference.
        @param adc ADC counts.
        @param tdc TDC counts.
        @returns digits entry. 
        """
        return {"adc_counts":adc, 
                "channel_id":{"type":"Tracker"}, 
                "tdc_counts":tdc}

    def __test_image_type(self, image_type):
        """
        Test worker's process method can create images of the
        specified type. birth is called first to set up the image
        type.
        @param self Object reference.
        @param image_type Image type e.g. "eps".
        @returns JSON document string from process method.
        """
        self.__reducer = ReducePyMatplotlibHistogram()
        success = self.__reducer.birth(
            """{"histogram_image_type":"%s"}""" % image_type)
        self.assertTrue(success, "reducer.birth() failed")
        self.__process({"digits":[self.__get_digit(1, 1)]})

    def __process(self, json_doc):
        """
        Convert given JSON document to a string and pass to
        the worker's process method.
        @param self Object reference.
        @param json_doc JSON document.
        """
        json_str = json.dumps(json_doc)
        result_str = self.__reducer.process(json_str)
        return json.loads(result_str)

    def __check_result(self, spill_id, result):
        """
        Validate results from worker's process method. Check
        the current spill count in the worker and the image names
        are as expected.        
        @param self Object reference.
        @param spill_id ID of spill just processed.
        @param result JSON document from process method.
        """
        self.assertEquals(spill_id + 1, self.__reducer.spill_count,
            "Unexpected reducer.spill_count")
        self.assertTrue("images" in result, "No images field")
        self.assertEquals(2, len(result["images"]),
            "Unexpected number of images")
        self.__check_image(result["images"][0], "%dspill" % spill_id)
        self.__check_image(result["images"][1], "%dspills" % spill_id)

    def __check_image(self, image, tag):
        """
        Check image entry as in JSON document from worker's process
        method. This assumes the image is an EPS image.
        @param self Object reference.
        @param image Image entry.
        @param tag Expected tag.
        """
        self.assertEquals(self.__reducer.image_type, image["image_type"],
            "Unexpected image_type")
        self.assertEquals(tag, image["tag"], "Unexpected tag")
        self.assertTrue("content" in image, "No content field")
        self.assertTrue("data" in image, "No data field")
        decoded_data = base64.b64decode(image["data"])
        self.assertTrue(decoded_data.find("EPS") != -1,
            "Unexpected image data")

    def tearDown(self):
        """
        Invoke worker's death method and remove any temporary 
        directories and image files.
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
