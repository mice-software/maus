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

class  test_gdml_formatter(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    class test_gdml_formatter This ensures that 
    GDMLFormatter.py is working as expected.
    """
    def setUp(self): #pylint: disable = C0103
        """
        method set_up
        
        This method creates a Formatter object ready for testing
        """
        self.constructor = None
        path = '/src/common_py/geometry/testCases/testGeometry'
        self.test_case = os.environ['MAUS_ROOT_DIR'] + path
        self.gdml = Formatter(self.test_case)

    def test_constructor(self):
        """
        method test_constructor 
        
        This method tests the Formatter constructor
        
        The first test tries to create a Formatter object
        with an integer which isn't allowed! The Formatter
        class should raise an error. 
        """
        try:
            self.constructor = Formatter(1)
            self.assertTrue(False, 'should have raised an exception')
        except:
            pass #pylint: disable = W0702
        
        #Now it tests the Formatter constructor by passing an
        #argument of a file which isn't a GDML. An error
        #should be raised.
        try:
            path = '/src/common_py/geometry/testCases/testFormatter'
            test_case = os.environ['MAUS_ROOT_DIR'] + path
            self.constructor = Formatter(test_case)
            self.assertTrue(False, 'should have raised an exception')
        except:
            pass #pylint: disable = W0702
        
        #this next section tests to make sure 
        #the class object is created properly
        err_msg = 'File was not saved as class object'
        materialfile = self.test_case + '/fastradModel_materials.xml'
        self.assertEqual(self.gdml.materialfile, materialfile, err_msg)
        configfile = self.test_case + '/fastradModel.gdml'
        self.assertEqual(self.gdml.configurationfile, configfile, err_msg)
        field_file = self.test_case + '/Field.gdml'
        self.assertEqual(self.gdml.field_file, field_file, err_msg)
        beamline_file = self.test_case + '/Beamline.gdml'
        self.assertEqual(self.gdml.beamline_file, beamline_file, err_msg)
        
    def test_format_schema_location(self):
        """
        method test_format_schema_location
        
        This passes a file which is not a GDML to 
        the relating method which should raise an
        error.
        """
        try:
            self.gdml.format_schema_location('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702
        
        #this next section calls format_schema_location
        #and checks the schema has been entered
        fin = open(self.gdml.configurationfile, 'r')
        count = 0
        for lines in fin.readlines():
            if lines.find(self.gdml.schema) >= 0:
                count += 1
        self.assertEqual(1, count, 'Formatting not complete')
        count = 0
        
    def test_merge_maus_info(self):
        """
        method test_merge_maus_info
        
        This method tests merge maus info by 
        checking to see if errors are raised
        when false arguments are entered.
        """
        try:
            self.gdml.merge_maus_info('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702
        
    def test_merge_run_info(self):
        """
        method test_merge_run_info
        
        This method tests merge maus info by 
        checking to see if errors are raised
        when false arguments are entered.
        """
        try:
            self.gdml.merge_run_info('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702

        fin = open(self.gdml.field_file, 'r')
        count = 0
        for lines in fin.readlines():
            if lines.find('run') >= 0:
                count += 1
        self.assertEqual(2, count, 'Formatting not complete')
        count = 0
                     
    
    def test_format_materials(self):
        """
        method test_format_materials
        
        This pass a file which is not a GDML to 
        the relating method which should raise an
        error.
        """
        try:
            self.gdml.format_materials('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702

    def test_insert_materials_ref(self):
        """
        method test_insert_materials_ref
        
        This pass a file which is not a GDML to 
        the relating method which should raise an
        error.
        """
        try:
            self.gdml.insert_materials_ref('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702   

    def test_format_check(self):
        """
        method test_format_check
        
        This pass a file which is not a GDML to 
        the relating method which should raise an
        error.
        """
        try:
            self.gdml.format_check('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass #pylint: disable = W0702
                
    def test_format(self):
        """
        method test_format
        
        This method tests the format method.
        It does this by firstly calling the 
        format method and then it opens the outputted
        (formatted) files. The input files are known and 
        the output files should be of a known form. These
        files are read in and certain lines which are present
        are counted and compared to numbers which we are
        known. 
        """
        self.gdml.format()
        count = 0
        fin = open(self.gdml.configurationfile, 'r')
        for lines in fin.readlines():
            if lines.find(self.gdml.schema) >= 0:
                count += 1
            if lines.find(self.gdml.materialfile) >= 0:
                count += 1
            if lines.find('<!-- Formatted for MAUS -->') >= 0:
                count += 1
        self.assertEqual(3, count, 'Formatting not complete')
        count = 0
        fin.close()
        length = len(self.gdml.stepfiles)
        for num in range(0, length):
            fin = open(self.gdml.stepfiles[num], 'r')
            for lines in fin.readlines():
                if lines.find(self.gdml.schema) >= 0:
                    count += 1
                if lines.find(self.gdml.materialfile) >= 0:
                    count += 1
                if lines.find('<!-- Formatted for MAUS -->') >= 0:
                    count += 1
            self.assertEqual(3, count, 'Formatting not complete')
            count = 0
            fin.close()
        
        
if __name__ == '__main__':
    unittest.main()