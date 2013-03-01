#pylint: disable =C0103
"""test_MapPyPrint.py"""
import json
import unittest

from MapPyPrint import MapPyPrint

class MapPyPrintTestCase(unittest.TestCase):#pylint: disable =R0904
    """MapPyPrintTestCase"""
    def test_empty(self):
        """test_empty"""
        mapper = MapPyPrint()
        self.assertTrue(mapper.birth("{}"))
        result = mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue(mapper.death())

    def test_return(self):
        """test_return"""
        mapper = MapPyPrint()
        self.assertTrue(mapper.birth("{}"))
        result = mapper.process("{}")  
        self.assertEqual(result, "{}")
        self.assertTrue(mapper.death())

if __name__ == '__main__':
    unittest.main()
