"""
Test class for ReducePyMatplotlibHistogram.
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

import base64
import json
import unittest

from ReducePyMatplotlibHistogram import ReducePyMatplotlibHistogram

class ReducePyMatplotlibHistogramTestCase(
    unittest.TestCase): # pylint: disable=C0103, R0904

    @classmethod
    def setUpClass(self): # pylint: disable=C0103, C0202
        self.__reducer = ReducePyMatplotlibHistogram()

    def setUp(self): # pylint: disable=C0103
        success = self.__reducer.birth("{}")
        if not success:
            raise Exception('Test setUp failed', 'reducer.birth() failed')

    def test_birth_default(self):
        self.assertEquals(0, self.__reducer.spill_count, 
            "Unexpected reducer.spill_count")
        self.assertEquals("eps", self.__reducer.image_type, 
            "Unexpected reducer.image_type")

    def test_birth_file_type(self):
        self.__reducer = ReducePyMatplotlibHistogram()
        success = self.__reducer.birth("""{"histogram_image_type":"png"}""")
        self.assertTrue(success, "reducer.birth() failed")
        self.assertEquals("png", self.__reducer.image_type, 
            "Unexpected reducer.image_type")

    def test_birth_bad_file_type(self):
        self.__reducer = ReducePyMatplotlibHistogram()
        with self.assertRaisesRegexp(ValueError,
            ".*Unsupported histogram image type.*"):
            self.__reducer.birth(
                """{"histogram_image_type":"no_such_extension"}""")

    def test_invalid_json(self):
        result_str = self.__reducer.process("", "{")
        result = json.loads(result_str)
        self.assertTrue("errors" in result, "No errors field")
        self.assertTrue("bad_json_document" in result["errors"],
            "No bad_json_document field")        

    def test_no_digits(self):
        result = self.__process({})
        self.assertTrue("errors" in result, "No errors field")
        self.assertTrue("no_digits" in result["errors"],
            "No no_digits field")        

    def test_empty_spill(self):
        json_doc = {"digits":{}}
        result = self.__process(json_doc)
        self.__check_result(0, 1, result)

    def test_no_channel_id(self):
        json_doc = {"digits": {"ac_counts":22, "tc_counts":22}}
        result = self.__process(json_doc)
        self.__check_result(0, 1, result)

    def test_no_tracker(self):
        json_doc = {"digits":[{"adc_counts":1, 
            "channel_id":{}, "tdc_counts":1}]}
        result = self.__process(json_doc)
        self.__check_result(0, 1, result)

    def test_no_matching_tracker(self):
        json_doc = {"digits":[{"adc_counts":1,
             "channel_id":{"type":"UnknownTracker"}, "tdc_counts":1}]}
        result = self.__process(json_doc)
        self.__check_result(0, 1, result)

    def test_adc_only(self):
        json_doc = {"digits":[{"adc_counts":1, 
            "channel_id":{"type":"Tracker"}}]}
        result = self.__process(json_doc)
        self.__check_result(0, 1, result)

    def test_tdc_only(self):
        json_doc = {"digits": [{"tdc_counts":1, 
            "channel_id":{"type":"Tracker"}}]}
        result = self.__process(json_doc)
        self.__check_result(0, 1, result)

    def test_multiple_spills_digits(self):
        for i in range(0, 4):
            json_doc = {"digits":[]}
            for j in range(0, 10):
                json_doc["digits"].append(self.__get_digit(j, j))
            result = self.__process(json_doc)
            self.__check_result(i, i + 1, result)

    def test_svg(self):
        self.__test_image_type("svg")

    def test_svgz(self):
        self.__test_image_type("svgz")

    def test_ps(self):
        self.__test_image_type("ps")

    def test_rgba(self):
        self.__test_image_type("rgba")

    def test_raw(self):
        self.__test_image_type("raw")

    def test_pdf(self):
        self.__test_image_type("pdf")

    def test_png(self):
        self.__test_image_type("png")

    def __get_digit(self, adc, tdc):
        return {"adc_counts":adc, 
                "channel_id":{"type":"Tracker"}, 
                "tdc_counts":tdc}

    def __test_image_type(self, image_type):
        self.__reducer = ReducePyMatplotlibHistogram()
        success = self.__reducer.birth(
            """{"histogram_image_type":"%s"}""" % image_type)
        self.assertTrue(success, "reducer.birth() failed")
        self.__process({"digits":[self.__get_digit(1, 1)]})

    def __process(self, json_doc):
        json_str = json.dumps(json_doc)
        result_str = self.__reducer.process("", json_str)
        return json.loads(result_str)

    def __check_result(self, spill_id, spill_count, result):
        self.assertEquals(spill_count, self.__reducer.spill_count,
            "Unexpected reducer.spill_count")
        self.assertTrue("images" in result, "No images field")
        self.assertEquals(2, len(result["images"]),
            "Unexpected number of images")
        self.__check_image(result["images"][0], "%dspill" % spill_id)
        self.__check_image(result["images"][1], "%dspills" % spill_id)

    def __check_image(self, image, tag):
        self.assertEquals(self.__reducer.image_type, image["image_type"],
            "Unexpected image_type")
        self.assertEquals(tag, image["tag"], "Unexpected tag")
        self.assertTrue("content" in image, "No content field")
        self.assertTrue("data" in image, "No data field")
        decoded_data = base64.b64decode(image["data"])
        self.assertTrue(decoded_data.find("EPS") != -1,
            "Unexpected image data")

    def tearDown(self): # pylint: disable=C0103
        success = self.__reducer.death()
        if not success:
            raise Exception('Test setUp failed', 'reducer.death() failed')

    @classmethod
    def tearDownClass(self): # pylint: disable=C0103, C0202
        self.__reducer = None

if __name__ == '__main__':
    unittest.main()
