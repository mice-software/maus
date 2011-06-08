import unittest
from GDMLPacker import packer


class  Test_GDMLPackerTestCase(unittest.TestCase):
    def setUp(self):
        self.GDML_test_case = packer("geom1.txt")
    
    def test_zipfile(self):
    
    #def tearDown(self):
    #    self.foo.dispose()
    #    self.foo = None

if __name__ == '__main__':
    unittest.main()

