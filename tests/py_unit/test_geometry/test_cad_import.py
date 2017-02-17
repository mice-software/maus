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

import os.path
import unittest
from geometry.CADImport import CADImport

class test_cad_import(unittest.TestCase): #pylint: disable = C0103, R0904
    """
    class test_cad_import This class tests CADImport.py
    
    This is the unit test which verifies CADImport.py
    is working correctly.
    """
    def setUp(self): #pylint: disable = C0103
        """
        TestCADImport::set_up This sets some variable for the coming tests
        
        This method sets up some CADImport objects which various
        variables in order to test the module.
        """
        self.maus = os.environ['MAUS_ROOT_DIR']
        self.constuctor = None
        test_geometry = '/tests/py_unit/test_geometry/testCases/testGeometry/'
        fra = test_geometry+'fastradModel.gdml'
        mag = test_geometry+'FieldInfoTest.xml'
        xslt = '/src/common_py/geometry/xsltScripts/'+\
                                        'RotatedGeometryFileTranslation.xsl'
        output = '/tmp/OUTPUTFILE.txt'
        self.xml_in_1 = self.maus + fra
        self.xml_in_2 = self.maus + mag
        self.xslt = self.maus + xslt
        self.out_put = self.maus + output
        self.cadimport_two_xml = CADImport(xmlin1=self.xml_in_1, \
           xsl=self.xslt, xmlin2=self.xml_in_2, output=self.out_put)

    def test_constructor(self): #pylint: disable = R0912, R0915
        """
        TestCADImport::test_constructor

        This method test the CADImport constructor
        
        Test xmlin1 parameter,
        first,   test it raises an error when there is no input
        second,  test it raises an error when it isn't an xml or gdml file
        """
        try:
            # pylint: disable = E1120
            self.constuctor = CADImport(xsl = self.xslt)
            self.assertTrue(False, "should have raised an exception")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        try:
            fast = "fastradModel.not_xml"
            self.constuctor = CADImport(xmlin1 = fast, xsl = self.xslt)
            self.assertTrue(False, "Should have raised an exception")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        #Test xsl parameter,
        #first,   test it raises an error when there is no input
        #second,  test it raises an error when it isn't an xsl file
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1)
            err = "Should have raised an exception"
            self.assertEqual(self.constuctor.xsl, None, err)
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        try:
            xfile = "GDML2G4MICE.not_xsl"
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = xfile)
            self.assertTrue(False, "Should have raised an exception")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        #Test xmlin2 parameter,
        #first,   test its default is none
        #second,  test it raises an error when it isn't a string
        #third,   test it raises an error when it isn't an xml or gdml file
        try:
            self.constuctor = CADImport(xmlin1=self.xml_in_1, xsl = self.xslt)
            err = "default is not None"
            self.assertEqual(self.constuctor.xml_in_2, None, err)
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        try:
            self.constuctor = CADImport(xmlin1=self.xml_in_1, \
                                        xsl=self.xslt, xmlin2=1)
            self.assertTrue(False, "Should have raised an exception")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        try:
            xfile = "FieldInfoTest.not_xml"
            self.constuctor = CADImport(xmlin1=self.xml_in_1, \
                                    xsl=self.xslt, xmlin2=xfile)
            self.assertTrue(False, "Should have raised an exception")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        #Test output parameter,
        #first,   test its default is none
        #second,  test it raises an error when it isn't a string
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt)
            err = "default is not None"
            self.assertEqual(self.constuctor.output, None, err)
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, \
                                      xsl = self.xslt, output = 1)
            self.assertTrue(False, "Should have raised an exception")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        #Test mergein parameter,
        #first,   test its default is Merge.xsl.in
        #second,  test it raises an error when it isn't a string
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, \
                     xsl = self.xslt, mergein = "something else")
            self.assertEqual(self.constuctor.merge_in, \
                   "Merge.xsl.in", "default is not Merge.xsl.in")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        
        try :
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, \
                                     xsl = self.xslt, mergin = 1)
            self.assertTrue(False, "Should have raised an exception")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        #Test mergeout parameter,
        #first,   test its default is Merge.xsl
        #second,  test it raises an error when it isn't a string
        #third,   test it raises an error when it isn't an xsl
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt)
            self.assertEqual(self.constuctor.merge_out, \
                  "Merge.xsl", "default is not Merge.xsl")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, \
                                    xsl = self.xslt, mergout = 1)
            self.assertTrue(False, "Should have raised an exception")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, \
                 xsl = self.xslt, mergeout = "MergedOut.not_xsl")
            self.assertTrue(False, "Should have raised an exception")
        except: #pylint: disable = W0702
            pass #pylint: disable = W0702


    def test_parse_xslt(self):
        """
        TestCADImport::test_parse_xslt
        Test to check that the outputted file has got information within it
        """
        self.cadimport_two_xml.parse_xslt()
        output_file = self.cadimport_two_xml.output
        self.assertTrue( os.path.getsize(output_file) != 0)
        #Test to check that the information 
        #within the outputted file is of MAUS Module format
        count = 0
        fin = open(output_file, 'r')
        for lines in fin.readlines():
            #line below needs to be reviewed as each file is different
            if lines.find('Configuration') >= 0 \
               or lines.find('Dimensions') >= 0 \
                 or lines.find('Position') >= 0 \
                  or lines.find('Rotation') >= 0:
                count += 1
        if count == 0:
            self.assertTrue(False, "output file is not of MAUS Module format")
        os.remove(self.out_put)      

        
if __name__ == '__main__':
    unittest.main()
