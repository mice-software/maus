"""
This file tests GDMLtoMAUSModule.py
M.Littlefield
"""
import os
import unittest
import shutil
from geometry.GDMLtoMAUSModule import GDMLtomaus

#pylint: disable = C0301, C0103, W0702, R0904, W0201
#I'm not sure what these errors mean will
#need to check with someone (Littlefield)

class  test_gdml_to_maus_module(unittest.TestCase):
    """
    class test_gdml_to_maus_module
    
    This class tests GDMLtoMAUSModule 
    to ensure it is working correctly.
    """    
    def setUp(self):
        """
        method set_up
        
        This method sets up a GDMLtomaus object
        ready for testing. It also has to move
        some files around because calling the methods
        from the module overwrites the original files. 
        Therefore copies of the testGeometry are made,
        tested and then removed.
        """
        self.constructor = None
        self.file_source = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/mausModuleSource'
        self.copy_files = GDMLtomaus(self.file_source)
        config_file = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGDMLtoMAUSModule/fastradModel.gdml'
        shutil.copyfile(self.copy_files.config_file, config_file)
        length = len(self.copy_files.step_files)
        for num in range (0, length):
            step_file = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGDMLtoMAUSModule/Step_' + str(num) + '.gdml'
            shutil.copyfile(self.copy_files.step_files[num], step_file)
        self.testcase = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGDMLtoMAUSModule'
        self.test_conversion = GDMLtomaus(self.testcase)
        
    def test_constructor(self):
        """
        method test_constructor
        
        This method tests the constructor
        by passing invalid arguments and seeing
        if errors are raised as they should be.
        """
        try:
            self.constructor = GDMLtomaus('this path doesnt exist')
            self.assertTrue(False, 'should have raised and error')
        except:
            pass
        #This next section ensures the constructor has set
        #up the variables correctly.
        config_file = self.testcase + '/fastradModel.gdml'
        self.assertEqual(self.test_conversion.config_file, config_file, 'Config file wasnt found, test_GDML_to_MAUSModule::test_constructor')
        num_of_step_files = len(self.test_conversion.step_files)
        self.assertEqual(num_of_step_files, 6, 'Step files wasnt found, test_GDML_to_MAUSModule::test_constructor')
        
    def test_convert_to_maus(self):
        """
        method test_convert_to_maus
        
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
            if fname == 'ParentGeometryFile.txt':
                config_mm = True
            if fname.find('Step') >= 0:
                step_files += 1
        self.assertTrue(config_mm, 'Configuration File not converted, test_GDML_to_MAUSModule::test_convert_to_maus')
        self.assertEqual(step_files, 6, 'Step Files not converted, test_GDML_to_MAUSModule::test_convert_to_maus')
        length = len(outputs)
        for num in range(0, length):
            path = self.testcase + '/' + outputs[num]
            os.remove(path)
        #NB: once the schema and XSLT scripts are finalised more test can be created here
        #These test can open the files and make sure the maus modules are written
        #correctly.
        
if __name__ == '__main__':
    unittest.main()

