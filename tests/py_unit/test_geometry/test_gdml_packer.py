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

import os
import unittest
from geometry.GDMLPacker import Packer
from geometry.GDMLPacker import Unpacker

#pylint: disable = C0301, C0103, W0702, R0904, W0201
#I'm not sure what these errors mean will
#need to check with someone (Littlefield)

class  test_packer(unittest.TestCase):
    """
    class test_packer 
    
    This class tests the packer class from the
    GDMLPacker.py file to ensure it is
    working correctly.
    """
    def setUp(self):
        """
        method set_up
        
        This method set up a Packer object
        ready for testing.
        """
        self.constructor_test = None
        testcase = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testPacker/FileList.txt'
        fin = open(testcase, 'w')
        fin.write(os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry/Step_0.gdml \n')
        fin.write(os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry/Step_1.gdml \n')
        fin.write(os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry/Step_2.gdml \n')
        fin.write(os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry/Step_3.gdml \n')
        fin.write(os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry/Step_4.gdml \n')
        fin.write(os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry/Step_5.gdml \n')
        fin.write(os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry/fastradModel_materials.xml \n')
        fin.write(os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry/fastradModel.gdml \n')
        fin.close()
        self.gdml_test_case = Packer(testcase)
    
    def test_constructor(self):
        """
        method test_constructor
        
        This method tests the constructor by passing argument 
        which are not valid and seeing if errors are raised.
        """
        try:
            self.constructor_test = Packer("geometry.not_txt")
            self.assertTrue(False, "should have raised an exception, test_GDMLPacker::test_constructor")
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
        mehod test_zipfile
        
        This method tests the zipfile method by 
        calling the method and checking the file size
        i.e. make sure its not empty.
        """
        zippath = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testPacker'
        self.gdml_test_case.zipfile(path = zippath)
        output = os.listdir(zippath)
        for fname in output:
            if fname[-4:] == '.zip':
                zfile = zippath + '/' + fname
        self.assertTrue(os.path.getsize(zfile) != 0, "zipped file size is zero, test_GDMLPacker::test_zipfile")

class test_unpacker(unittest.TestCase):
    """
    class test_unpacker
    
    This class test the unpacker class
    from GDMLPacker.py 
    """
    def setUp(self):
        """
        method set_up
        
        This method sets up an Unpacker
        object ready for testing.
        """
        self.constructor_test = None
        zippath = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testPacker'
        output = os.listdir(zippath)
        for fname in output:
            if fname[-4:] == '.zip':
                zfile = zippath + '/' + fname
        self.testpath = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases'
        self.extraction_test = Unpacker(zfile, self.testpath)
        
    def test_constructor(self):
        """
        method test_constructor
        
        This method tests the constructor by passing argument 
        which are not valid and seeing if errors are raised.
        """
        try:
            self.constructor_test = Unpacker('Geometry.not_zip', self.testpath)
            self.assertTrue(False, 'Should have raised an error, Test_Unpacker::test_constructor')
        except:
            pass
        
        try: 
            self.constructor_test = Unpacker(self.testpath, 'Not_a_path')
            self.assertTrue(False, 'Should have raised an error, Test_Unpacker::test_constructor')
        except:
            pass
        
    def test_unzip_file(self):
        """
        method test_unzip_file
        
        This method tests the unzip_file method. It does this
        by unzipping the testcase zipfile and checking that the 
        correct files are there.
        """
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