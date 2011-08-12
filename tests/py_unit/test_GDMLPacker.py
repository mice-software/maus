import os
import os.path
import unittest
from geometry.GDMLPacker import Packer


class  Test_GDMLPackerTestCase(unittest.TestCase):
    def setUp(self):
        self.constructor_test = None
        testcase = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testPacker/FileList.txt'
        self.gdml_test_case = Packer(testcase)
    
    def test_constructor(self):
        """
        this test will check the constructor raises appropriate errors.
        """
        try:
            self.constructor_test = packer("geometry.not_txt")
            self.asserttrue(False, "should have raised an exception, test_GDMLPacker::test_constructor")
        except:
            pass

        try:
            blank_txt = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testPacker'
            self.constructor_test = packer(blank_txt)
            self.assertTrue(False, "should have raised an exception, test_GDMLPacker::test_constructor")
        except:
            pass

    def test_zipfile(self):
        """
        this test checks to see if the outputted zip file has been written
        """
        zippath = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testPacker'
        self.gdml_test_case.zipfile(path = zippath)
        output = os.listdir(zippath)
        for fname in output:
            if fname[-4:] == '.zip':
                zfile = zippath + '/' + fname
        self.assertTrue(os.path.getsize(zfile) != 0, "zipped file size is zero, test_GDMLPacker::test_zipfile")
        os.remove(zfile)

    def tearDown(self):
        self.GDML_test_case = None

if __name__ == '__main__':
    unittest.main()
