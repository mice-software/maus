import os.path
import unittest
from GDMLPacker import packer


class  Test_GDMLPackerTestCase(unittest.TestCase):
    def setUp(self):
        self.Constructor_test = None
        self.GDML_test_case = packer("geom1.txt")
    
    def test_constructor(self):
        """
        this test will check the constructor raises appropriate errors.
        """
        try:
            self.Constructor_test = packer("geometry.not_txt")
            self.asserttrue(False, "should have raised an exception, test_GDMLPacker::test_constructor")
        except:
            pass

        try:
            self.Constructor_test = packer("test.txt")
            self.assertTrue(False, "should have raised an exception, test_GDMLPacker::test_constructor")
        except:
            pass

    def test_zipfile(self):
        """
        this test checks to see if the outputted zip file has been written
        """
        self.GDML_test_case.zipfile()
        self.assertTrue(os.path.getsize("GDML.zip") != 0, "zipped file size is zero, test_GDMLPacker::test_zipfile")
        

    def tearDown(self):
        self.GDML_test_case = None

if __name__ == '__main__':
    unittest.main()

