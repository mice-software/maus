#pylint:disable=C0103
"""test_MapCppPrint.py"""
import json
import unittest

from MapCppPrint import MapCppPrint

class MapCppPrintTestCase(unittest.TestCase):#pylint:disable=R0904
    """MapCppPrintTestCase"""
    @classmethod
    def setUpClass(self):#pylint:disable=C0103,C0202
        """setUpClass"""
        self.mapper = MapCppPrint()
        success = self.mapper.birth("{}")
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')

    def test_empty(self):
        """test_empty"""
        result = self.mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue("bad_json_document" in doc["errors"])

    @classmethod
    def tearDownClass(self):#pylint:disable=C0103,C0202
        """tearDownClass"""
        success = self.mapper.death()
        if not success:
            raise Exception('InitializeFail', 'Could not start worker')
        self.mapper = None

if __name__ == '__main__':
    unittest.main()
