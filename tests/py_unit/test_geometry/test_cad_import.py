"""
This class test the CADImport.py module
M. Littlefield
"""
import os.path
import unittest
from geometry.CADImport import CADImport

#pylint: disable = C0301, C0103, W0702, R0912, R0915, R0904, W0201
# Some of these disabled errors I don't understand need to look
# at them with someone who has experience. (Littlefield)

class TEST_cad_import(unittest.TestCase):
    """
    class test_cad_import This class tests CADImport.py
    
    This is the unit test which verifies CADImport.py
    is working correctly.
    """
    def set_up(self):
        """
        method set_up This sets some variable for the coming tests
        
        This method sets up some CADImport objects which various
        variables in order to test the module.
        """
        self.maus = os.environ['MAUS_ROOT_DIR']
        self.constuctor = None
        self.xml_in_1 = self.maus + '/src/common_py/geometry/testCases/testGeometry/fastradModel.gdml'
        self.xml_in_2 = self.maus + '/src/common_py/geometry/testCases/testGeometry/FieldInfoTest.xml'
        self.xslt = self.maus + '/src/common_py/geometry/xsltScripts/GDML2G4MICE.xsl'
        self.out_put = self.maus + '/src/common_py/geometry/testCases/OUTPUTFILE.txt'
        self.cadimport_two_xml = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt, xmlin2 = self.xml_in_2, output = self.out_put)

    def test_constructor(self):
        """
        method test_constructor This method test the CADImport constructor
        
        Test xmlin1 parameter,
        first,   test it raises an error when there is no input
        second,  test it raises an error when it isn't an xml or gdml file
        """
        try:
            self.constuctor = CADImport(xsl = self.xslt)
            self.assertTrue(False, "should have raised an exception")
        except:
            pass
        try:
            self.constuctor = CADImport(xmlin1 = "fastradModel.not_xml", xsl = self.xslt)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        #Test xslin1 parameter,
        #first,   test it raises an error when there is no input
        #second,  test it raises an error when it isn't an xsl file
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = "GDML2G4MICE.not_xsl")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        #Test xmlin2 parameter,
        #first,   test its default is none
        #second,  test it raises an error when it isn't a string
        #third,   test it raises an error when it isn't an xml or gdml file
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt)
            self.assertEqual(self.constuctor.xml_in_2, None, "default is not None")
        except:
            pass
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt, xmlin2 = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt, xmlin2 = "FieldInfoTest.not_xml")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        #Test output parameter,
        #first,   test its default is none
        #second,  test it raises an error when it isn't a string
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt)
            self.assertEqual(self.constuctor.output, None, "default is not None")
        except:
            pass
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt, output = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        #Test mergein parameter,
        #first,   test its default is Merge.xsl.in
        #second,  test it raises an error when it isn't a string
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt, mergein = "something else")
            self.assertEqual(self.constuctor.merge_in, "Merge.xsl.in", "default is not Merge.xsl.in")
        except:
            pass
        
        try :
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt, mergin = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        #Test mergeout parameter,
        #first,   test its default is Merge.xsl
        #second,  test it raises an error when it isn't a string
        #third,   test it raises an error when it isn't an xsl
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt)
            self.assertEqual(self.constuctor.merge_out, "Merge.xsl", "default is not Merge.xsl")
        except:
            pass
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt, mergout = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        try:
            self.constuctor = CADImport(xmlin1 = self.xml_in_1, xsl = self.xslt, mergeout = "MergedOut.not_xsl")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass


    def test_parse_xslt(self):
        """
        Test to check that the outputted file has got information within it
        """
        self.cadimport_two_xml.parse_xslt()
        output_file = self.cadimport_two_xml.output
        self.assertTrue( os.path.getsize(output_file) != 0)
        #Test to check that the information within the outputted file is of MAUS Module format
        count = 0
        fin = open(output_file, 'r')
        for lines in fin.readlines():
            #line below needs to be reviewed as each file is different
            if lines.find('Configuration') >= 0 or lines.find('Dimensions') >= 0 or lines.find('Position') >= 0 or lines.find('Rotation') >= 0:
                count += 1
        if count == 0:
            self.assertTrue(False, "output file is not of MAUS Module format")
        os.remove(self.out_put)

    def test_append_merge(self):
        """
        This method test append_merge
        """
        self.cadimport_two_xml.append_merge()
        #this test check to see if the word EDIT was replaced in Merge.xsl.in by
        #checking the output file Merge.xsl for the word EDIT
        count1 = 0
        fout = open(self.cadimport_two_xml.merge_out, 'r')
        for line in fout.readlines():
            if line.find('EDIT')>=0:
                count1 += 1
        self.assertTrue(count1 == 0, "EDIT was not replaced in Merge.xsl")
        #this tests to make sure there is only on EDIT in the Merge.xsl.in file
        count2 = 0
        fin = open(self.cadimport_two_xml.merge_in, 'r')
        for line in fin.readlines():
            if line.find('EDIT')>=0:
                count2 += 1
        self.assertTrue(count2 == 1, "More than one EDIT found in Merge.xsl.in")
        os.remove(self.cadimport_two_xml.merge_out)
        
        
    
if __name__ == '__main__':
    unittest.main()
