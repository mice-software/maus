import os
import unittest
import shutil
from geometry.GDMLtoMAUSModule import GDMLtomaus

class  test_GDML_to_MAUSModule(unittest.TestCase):
    
    def setUp(self):
        self.constructor = None
        self.file_source = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry'
        self.copy_files = GDMLtomaus(self.file_source)
        config_file = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGDMLtoMAUSModule/fastradModel.gdml'
        shutil.copyfile(self.copy_files.config_file, config_file)
        length = len(self.copy_files.step_files)
        for num in range (0, length):
            step_file = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGDMLtoMAUSModule/Step_' + [num] + '.gdml'
            shutil.copyfile(self.copy_files.step_files[num], step_file)
        self.testcase = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGDMLtoMAUSModule'
        self.test_conversion = GDMLtomaus(self.testcase)
        
    def test_constructor(self):
        try:
            self.constructor = GDMLtomaus('this path doesnt exist')
            self.assertTrue(False, 'should have raised and error')
        except:
            pass
        
        material_file = self.testcase + '/fastradModel_materials.xml'
        self.assertEqual(self.test_conversion.material_file, material_file, 'Material file wasnt found, test_GDML_to_MAUSModule::test_constructor')
        config_file = self.testcase + '/fastradModel.gdml'
        self.assertEqual(self.test_conversion.config_file, config_file, 'Config file wasnt found, test_GDML_to_MAUSModule::test_constructor')
        num_of_step_files = len(self.test_conversion.step_files)
        self.assertEqual(num_of_step_files, 6, 'Step files wasnt found, test_GDML_to_MAUSModule::test_constructor')
        
    def test_convert_to_maus(self):
        self.test_conversion.convert_to_maus(self.testcase)
        outputs = os.listdir(self.output)
        config_mm = False
        #material_file = False
        step_files = 0
        for fname in outputs:
            if fname == 'ParentGeometryFile.txt':
                config_mm = True
            #if fname ==  'fastradModel_materials.xml':
             #   material_file = True
            if fname.find('Step') >= 0:
                print fname
                print step_files
                step_files += 1
        self.assertTrue(config_mm, 'Configuration File not converted, test_GDML_to_MAUSModule::test_convert_to_maus')
        #self.assertTrue(material_file, 'Material File not converted, test_GDML_to_MAUSModule::test_convert_to_maus')
        self.assertEqual(step_files, 6, 'Step Files not converted, test_GDML_to_MAUSModule::test_convert_to_maus')
        print outputs
        
        
#    def tearDown(self):
 #       self.testcase = None
  #      self.testnote = None
   #     self.test_gdml_to_cdb =None

if __name__ == '__main__':
    unittest.main()

