"""
M. Littlefield
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

import os
import unittest
from geometry.GDMLPacker import Packer
from geometry.GDMLPacker import Unpacker

class  TestPacker(unittest.TestCase): #pylint: disable = R0904
    """
    class test_packer 
    
    This class tests the packer class from the
    GDMLPacker.py file to ensure it is
    working correctly.
    """
    def setUp(self): #pylint: disable = C0103
        """
        TestPacker::set_up
        
        This method set up a Packer object
        ready for testing.
        """
        self.constructor_test = None
        self.testcases_dir = os.environ['MAUS_ROOT_DIR'] + \
                                       '/tests/py_unit/test_geometry/testCases/'
        self.testgeom_dir = self.testcases_dir+'testGeometry/'
        self.testpacker_dir = self.testcases_dir+'testPacker/'
        testcase = self.testpacker_dir+'/FileList.txt'
        fin = open(testcase, 'w')
        fin.write(self.testgeom_dir+'/Step_0.gdml \n')
        fin.write(self.testgeom_dir+'/Step_1.gdml \n')
        fin.write(self.testgeom_dir+'/Step_2.gdml \n')
        fin.write(self.testgeom_dir+'/Step_3.gdml \n')
        fin.write(self.testgeom_dir+'/Step_4.gdml \n')
        fin.write(self.testgeom_dir+'/Step_5.gdml \n')
        fin.write(self.testgeom_dir+'/fastradModel_materials.xml \n')
        fin.write(self.testgeom_dir+'/fastradModel.gdml \n')
        fin.close()
        self.gdml_test_case = Packer(testcase)
    
    def test_constructor(self):
        """
        TestPacker::test_constructor
        
        This method tests the constructor by passing argument 
        which are not valid and seeing if errors are raised.
        """
        self.assertRaises(IOError, Packer, "geometry.not_txt")
        blank_txt = self.testpacker_dir+'test.txt'
        self.assertRaises(StandardError, Packer, blank_txt)

    def test_zipfile(self):
        """
        TestPacker::test_zipfile
        
        This method tests the zipfile method by 
        calling the method and checking the file size
        i.e. make sure its not empty.
        """
        zippath = self.testpacker_dir
        self.gdml_test_case.zipfile(path = zippath)
        output = os.listdir(zippath)
        for fname in output:
            if fname[-4:] == '.zip':
                zfile = zippath + '/' + fname
        self.assertTrue(os.path.getsize(zfile) != 0, \
        "zipped file size is zero, test_GDMLPacker::test_zipfile")

class TestUnpacker(unittest.TestCase): #pylint: disable = R0904
    """
    TestUnpacker
    
    This class test the unpacker class
    from GDMLPacker.py 
    """
    def setUp(self): #pylint: disable = C0103
        """
        method set_up
        
        This method sets up an Unpacker
        object ready for testing.
        """
        self.constructor_test = None
        self.testpath = os.environ['MAUS_ROOT_DIR'] + \
                                       '/tests/py_unit/test_geometry/testCases/'
        output = os.listdir(self.testpath)
        zfile = ''
        for fname in output:
            if fname[-4:] == '.zip':
                zfile = self.testpath + '/' + fname
        if zfile == '':
            raise RuntimeError('Expected to find zip file in '+self.testpath)
        self.extraction_test = Unpacker(zfile, self.testpath)
        
    def test_constructor(self):
        """
        TestUnpacker::test_constructor
        
        This method tests the constructor by passing argument 
        which are not valid and seeing if errors are raised.
        """
        self.assertRaises(IOError, Unpacker, 'Geometry.not_zip', self.testpath)
        self.assertRaises(IOError, Unpacker, self.testpath, 'Not_a_path')
        
    def test_unzip_file(self):
        """
        TestUnpacker::test_unzip_file
        
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
        self.assertEqual(text_file, self.testpath+'/FileList.txt', \
               'File not unzipped, Test_Unpakcer::test_unzip_file')
#        os.remove(self.extraction_test.input_file)
        os.remove(text_file)         

if __name__ == '__main__':
    unittest.main()
