import os
import os.path
import unittest
from geometry.GDMLPacker import Packer
from geometry.GDMLPacker import Unpacker


class  Test_Packer(unittest.TestCase):
    def setUp(self):
        self.constructor_test = None
        testcase = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testPacker/FileList.txt'
        self.gdml_test_case = Packer(testcase)
    
    def test_constructor(self):
        """
        this test will check the constructor raises appropriate errors.
        """
        try:
            self.constructor_test = Packer("geometry.not_txt")
            self.asserttrue(False, "should have raised an exception, test_GDMLPacker::test_constructor")
        except:
            pass

        try:
            blank_txt = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testPacker/test.txt'
            self.constructor_test = Packer(blank_txt)
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

    def tearDown(self):
        self.GDML_test_case = None
        
class Test_Unpacker(unittest.TestCase):
    
    def setUp(self):
        self.constructor_test = None
        zippath = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testPacker'
        output = os.listdir(zippath)
        for fname in output:
            if fname[-4:] == '.zip':
                zfile = zippath + '/' + fname
        testpath = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases'
        self.extraction_test = Unpacker(zfile, testpath)
        
    def test_constructor(self):
        try:
            self.constructor_test = Unpacker('Geometry.not_zip', testpath)
            self.assertTrue(False, 'Should have raised an error, Test_Unpacker::test_constructor')
        except:
            pass
        
        try: 
            self.constructor_test = Unpacker(testcase, 'Not_a_path')
            self.assertTrue(False, 'Should have raised an error, Test_Unpacker::test_constructor')
        except:
            pass
        
    def test_unzip_file(self):
        self.extraction_test.unzip_file()
        output = os.listdir(self.extraction_test.extract_path)
        for fname in output:
            if fname != 'FileList.txt' and fname[-5:] == '.gdml':
                fname = self.extraction_test.extract_path + '/' + fname
                os.remove(fname)
            if fname != 'FileList.txt' and fname[-4:] == '.xml':
                fname = self.extraction_test.extract_path + '/' + fname
                os.remove(fname)
            if fname == 'FileList.txt':
                fname = self.extraction_test.extract_path + '/' + fname
                text_file = fname
        self.assertEqual(text_file, os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/FileList.txt', 'File not unzipped, Test_Unpakcer::test_unzip_file')
        os.remove(self.extraction_test.input_file)
        os.remove(text_file)         

if __name__ == '__main__':
    unittest.main()