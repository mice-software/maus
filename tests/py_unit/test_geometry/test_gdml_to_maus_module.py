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
import shutil
from geometry.GDMLtoMAUSModule import GDMLtomaus

class  TestGDMLToMausModule(unittest.TestCase):#pylint:disable = C0103,R0904
    """
    class TestGDMLToMausModule
    
    This class tests GDMLtoMAUSModule 
    to ensure it is working correctly.
    """    
    def setUp(self): #pylint: disable = C0103
        """
        TestGDMLToMausModule::setUp
        
        This method sets up a GDMLtomaus object
        ready for testing. It also has to move
        some files around because calling the methods
        from the module overwrites the original files. 
        Therefore copies of the testGeometry are made,
        tested and then removed.
        """
        test_cases_dir = os.environ['MAUS_ROOT_DIR'] + \
                                       '/tests/py_unit/test_geometry/testCases/'
        self.constructor = None
        self.file_source = os.environ['MAUS_ROOT_DIR'] + '/tmp'
        self.copy_files = GDMLtomaus(self.file_source)
        config_file = test_cases_dir+'/testGDMLtoMAUSModule/fastradModel.gdml'
        shutil.copyfile(self.copy_files.config_file, config_file)
        length = len(self.copy_files.step_files)
        for num in range (0, length):
            step_file = test_cases_dir+'/testGDMLtoMAUSModule/Step_' + \
                                                              str(num) + '.gdml'
            shutil.copyfile(self.copy_files.step_files[num], step_file)
        self.testcase = test_cases_dir+'/testGDMLtoMAUSModule'
        self.test_conversion = GDMLtomaus(self.testcase)
        
    def test_constructor(self):
        """
        TestGDMLToMausModule::test_constructor
        
        This method tests the constructor
        by passing invalid arguments and seeing
        if errors are raised as they should be.
        """
        try:
            self.constructor = GDMLtomaus('this path doesnt exist')
            self.assertTrue(False, 'should have raised and error')
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        #This next section ensures the constructor has set
        #up the variables correctly.
        config_file = self.testcase + '/fastradModel.gdml'
        self.assertEqual(self.test_conversion.config_file, config_file, \
        'Config file wasnt found, test_GDML_to_MAUSModule::test_constructor')
        num_of_step_files = len(self.test_conversion.step_files)
        self.assertEqual(num_of_step_files, 6, \
        'Step files wasnt found, test_GDML_to_MAUSModule::test_constructor')
        
    def test_convert_to_maus(self):
        """
        TestGDMLToMausModule::test_convert_to_maus
        
        This method tests the convert_to_maus
        method which should convert the files to maus
        modules. It tests this be converting the files and
        checking the maus modules exist.
        """
        self.test_conversion.convert_to_maus(self.testcase)
        outputs = os.listdir(self.testcase)
        config_mm = False
        step_files = 0
        for fname in outputs:
            if fname == 'ParentGeometryFile.dat':
                config_mm = True
            if fname.find('Step') >= 0:
                step_files += 1
        self.assertTrue(config_mm, 'Configuration File not converted')
        self.assertEqual(step_files, 6, 'Step Files not converted')
        length = len(outputs)
        for num in range(0, length):
            path = self.testcase + '/' + outputs[num]
            os.remove(path)
        #NB: once the schema and XSLT scripts 
        #are finalised more test can be created here
        #These test can open the files  
        #and make sure the maus modules are written
        #correctly.
        
if __name__ == '__main__':
    unittest.main()

