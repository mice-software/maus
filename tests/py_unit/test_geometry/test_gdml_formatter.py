"""
M.littlefield
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

import unittest
import os
from geometry.GDMLFormatter import Formatter

class  TestGDMLFormatter(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    class TestGDMLFormatter This ensures that GDMLFormatter.py is working as
    expected.
    """
    def setUp(self): #pylint: disable = C0103
        """
        TestGDMLFormatter::set_up
        
        This method creates a Formatter object ready for testing
        """
        self.constructor = None
        path_in = '/tests/py_unit/test_geometry/testCases/mausModuleSource'
        self.path_in = os.environ['MAUS_ROOT_DIR'] + path_in
        self.path_out = os.path.join(os.environ['MAUS_ROOT_DIR'], 'tmp')
        self.gdml = Formatter(self.path_in, self.path_out)

    def test_constructor(self):
        """
        TestGDMLFormatter::test_constructor 
        
        This method tests the Formatter constructor
        
        The first test tries to create a Formatter object with an integer which
        isn't allowed. The Formatter class should raise an error. 
        """
        try:
            self.constructor = Formatter(1) # pylint: disable = E1120
            self.assertTrue(False, 'should have raised an exception')
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        
        #Now it tests the Formatter constructor by passing an
        #argument of a file which isn't a GDML. An error
        #should be raised.
        try:
            path = '/tests/py_unit/test_geometry/testCases/testFormatter'
            test_case = os.environ['MAUS_ROOT_DIR'] + path
            self.constructor = Formatter(test_case) # pylint: disable = E1120
            self.assertTrue(False, 'Should have raised an exception')
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        
        #this next section tests to make sure 
        #the class object is created properly
        materialfile = 'fastradModel_materials.xml'
        self.assertEqual \
                      (self.gdml.material_file, materialfile)
        configfile = 'fastradModel.gdml'
        self.assertEqual(self.gdml.configuration_file, configfile)
        maus_information_file = 'Maus_Information.gdml'
        self.assertEqual(self.gdml.maus_information_file, maus_information_file)
        beamline_file = 'Beamline.gdml'
        self.assertEqual(self.gdml.beamline_file, beamline_file)
        
    def test_format_schema_location(self):
        """
        TestGDMLFormatter::test_format_schema_location
        
        This passes a file which is not a GDML to the relating method which
        should raise an error.
        """
        #this next section calls format_schema_location
        #and checks the schema has been entered
        try:
            self.gdml.format_schema_location('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
   
    def test_merge_maus_info(self):
        """
        TestGDMLFormatter::test_merge_maus_info
        
        This method tests merge maus info by checking to see if errors are
        raised when false arguments are entered.
        """
        try:
            self.gdml.merge_maus_info('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        
    def test_merge_run_info(self):
        """
        TestGDMLFormatter::test_merge_run_info
        
        This method tests merge maus info by checking to see if errors are
        raised when false arguments are entered.
        """
        try:
            self.gdml.merge_run_info('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
                     
    def test_format_materials(self):
        """
        TestGDMLFormatter::test_format_materials
        
        This pass a file which is not a GDML to the relating method which should
        raise an error.
        """
        try:
            self.gdml.format_materials('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702

    def test_insert_materials_ref(self):
        """
        TestGDMLFormatter::test_insert_materials_ref
        
        This pass a file which is not a GDML to the relating method which should
        raise an error.
        """
        try:
            self.gdml.insert_materials_ref('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702   

    def test_format_check(self):
        """
        TestGDMLFormatter::test_format_check
        
        This pass a file which is not a GDML to the relating method which should
        raise an error.
        """
        try:
            self.gdml.format_check('Geometry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
                
    def test_format(self): #pylint: disable = R0912
        """
        TestGDMLFormatter::test_format
        
        This method tests the format method. It does this by firstly calling the 
        format method and then it opens the outputted (formatted) files. The
        input files are known and the output files should be of a known form.
        These files are read in and certain lines which are present are counted
        and compared to numbers which we are known. 
        """
        self.gdml.format()
        for gdml_file in [self.gdml.configuration_file]+self.gdml.stepfiles:
            (schema_found, material_found, formatted_found) = (0, 0, 0)
            (run_found, mice_found, path_out_found) = (0, 0, 0)
            filename = self.path_out+'/'+gdml_file
            fin = open(filename, 'r')
            if gdml_file == self.gdml.configuration_file:
                for line in fin.readlines():
                    if self.gdml.maus_information_file != None:
                        if line.find('</run>') >= 0:
                            run_found += 1
                    if self.gdml.beamline_file != None:
                        if line.find('</MICE_Information>') >= 0:
                            mice_found += 1
                    if line.find(self.gdml.schema) >= 0:
                        schema_found += 1
                    if line.find(self.gdml.material_file) >= 0:
                        material_found += 1
                    if line.find('<!-- Formatted for MAUS -->') >= 0:
                        formatted_found += 1
                    maus = 'location="' + self.gdml.path_out + '"'
                    if line.find(maus) >= 0:
                        path_out_found += 1
                self.assertEqual(run_found, 1,
                            msg="Run tag not found "+filename)
                self.assertEqual(mice_found, 1,
                            msg="MICE Info tag not found "+filename)
                self.assertEqual(path_out_found, 1,
                            msg="path_out tag not found "+filename)
            else:
                for line in fin.readlines():
                    if line.find(self.gdml.schema) >= 0:
                        schema_found += 1
                    if line.find(self.gdml.material_file) >= 0:
                        material_found += 1
                    if line.find('<!-- Formatted for MAUS -->') >= 0:
                        formatted_found += 1
            self.assertEqual(schema_found, 1, msg="Schema "+self.gdml.schema+\
                              " found "+str(schema_found)+" times in "+filename)
            self.assertEqual(material_found, 1,
                                             msg="Material not found "+filename)
            self.assertEqual(formatted_found, 1,
                                        msg="Formatted tag not found "+filename)

            fin.close()
        
if __name__ == '__main__':
    unittest.main()
