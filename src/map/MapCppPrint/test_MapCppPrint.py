import json
import unittest

from MapCppPrint import MapCppPrint

class MapCppPrintTestCase(unittest.TestCase):
    @classmethod
    def setUpClass(self):
        self.mapper = MapCppPrint()
        success = self.mapper.Birth("{}")
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

    def test_empty(self):
        result = self.mapper.Process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    @classmethod
    def tearDownClass(self):
        success = self.mapper.Death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()