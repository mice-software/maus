import json
import unittest

from MapPyPrint import MapPyPrint

class MapPyPrintTestCase(unittest.TestCase):
    def test_empty(self):
        mapper = MapPyPrint()
        self.assertTrue(mapper.birth("{}"))
        result = mapper.process("")
        doc = json.loads(result)
        self.assertTrue("errors" in doc)
        self.assertTrue(mapper.death())

    def test_return(self):
        mapper = MapPyPrint()
        self.assertTrue(mapper.birth("{}"))
        result = mapper.process("{}")  
        self.assertEqual(result, "{}")
        self.assertTrue(mapper.death())

if __name__ == '__main__':
    unittest.main()
