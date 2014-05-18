"""
R. Bayes
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

import os.path
import unittest
from geometry.CADImport import CADImport

class test_python_import(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    class test_python_import This class tests CADImport.py
    
    This is the unit test which verifies CADImport.py
    is working correctly.
    """
    def setUp(self): #pylint: disable = C0103
        """
        TestPythonImport::set_up This sets some variable for the coming tests
        
        This method sets up some CADImport objects which various
        variables in order to test the module.
        """
        self.maus = os.environ['MAUS_ROOT_DIR']
        self.constuctor = None
        test_geometry = '/tests/py_unit/test_geometry/testCases/testGeometry/'
        fra = test_geometry+'TOF2.gdml'
        #xslt = '/src/common_py/geometry/xsltScripts/'+\
        #                                'RotatedGeometryFileTranslation.xsl'
        output = '/tmp/TOF2FILE.txt'
        self.xml_in_1 = self.maus + fra
        # self.xslt = self.maus + xslt
        self.out_put = self.maus + output
        self.cadimport_one_xml = CADImport(xmlin1=self.xml_in_1, \
           output=self.out_put)

    def test_parse_python(self):
        """
        TestPythonImport::test_parse_python
        Test to check that the outputted file has useful information within it
        """
        self.cadimport_one_xml.translate_gdml()
        output_file = self.cadimport_one_xml.output
        self.assertTrue( os.path.getsize(output_file) != 0)
        #Test to check that the information 
        #within the outputted file is of MAUS Module format
        count = 0
        fin = open(output_file, 'r')
        for lines in fin.readlines():
            #line below needs to be reviewed as each file is different
            if lines.find('Volume') >= 0 \
               or lines.find('Dimensions') >= 0 \
                 or lines.find('Position') >= 0 \
                  or lines.find('Rotation') >= 0:
                count += 1
        if count == 0:
            self.assertTrue(False, "output file is not of MAUS Module format")
        os.remove(self.out_put)
        
if __name__ == '__main__':
    unittest.main()
