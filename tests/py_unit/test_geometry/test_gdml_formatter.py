"""
This module test the GDMLFormatter module
M. Littlefield
"""
import unittest
import os
from geometry.GDMLFormatter import Formatter

#pylint: disable = C0301, W0702, C0103, R0904, W0201

class  test_gdml_formatter(unittest.TestCase):
    """
    class test_gdml_formatter This ensures that 
    GDMLFormatter.py is working as expected.
    """
    def set_up(self):
        """
        method set_up
        
        This method creates a Formatter object ready for testing
        """
        self.constructor = None
        test_case = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry'
        self.gdml = Formatter(test_case)

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
            pass
        
        #Now it tests the Formatter constructor by passing an
        #argument of a file which isn't a GDML. An error
        #should be raised.
        try:
            test_case = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testFormatter'
            self.constructor = Formatter(test_case)
            self.assertTrue(False, 'should have raised an exception')
        except:
            pass
        
    def test_format_schema_location(self):
        """
        method test_format_schema_location
        
        This pass a file which is not a GDML to 
        the relating method which should raise an
        error.
        """
        try:
            self.gdml.format_schema_location('Gemoetry.not_gdml')
            self.assertTrue(False, 'Should have raised an error')
        except:
            pass
    
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
            pass

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
            pass    

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
            pass
                
    def test_format(self):
        """
        method test_format
        
        This method test the format method.
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