import os.path
import unittest
from geometry.CADImport import CADImport

class  test_CADImport(unittest.TestCase):
    def setUp(self):
        self.maus_root = os.environ['MAUS_ROOT_DIR']
        self.Constructor = None
        xml_in_1 = self.maus_root + '/src/common_py/geometry/testCases/testGeometry/fastradModel.gdml'
        xml_in_2 = self.maus_root + '/src/common_py/geometry/testCases/testGeometry/FieldInfoTest.xml'
        x_s_l = self.maus_root + '/src/common_py/geometry/xsltScripts/GDML2G4MICE.xsl'
        self.out_put = self.maus_root + '/src/common_py/geometry/testCases/OUTPUTFILE.txt'
        self.CADImport_two_xml = CADImport(xmlin1 = xml_in_1, xsl = x_s_l, xmlin2 = xml_in_2, output = self.out_put)

    def test_Constructor(self):
        """
        Test xmlin1 parameter,
        first,   test it raises an error when there is no input
        second,  test it raises an error when it isn't an xml or gdml file
        """
        try:
            self.Constructor = CADImport(xsl = x_s_l)
            self.assertTrue(False, "should have raised an exception")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.not_xml", xsl = x_s_l)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        """
        Test xslin1 parameter,
        first,   test it raises an error when there is no input
        second,  test it raises an error when it isn't an xsl file
        """
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = "GDML2G4MICE.not_xsl")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        """
        Test xmlin2 parameter,
        first,   test its default is none
        second,  test it raises an error when it isn't a string
        third,   test it raises an error when it isn't an xml or gdml file
        """
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l)
            self.assertEqual(self.Constructor.XMLIn2, None, "default is not None")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l, xmlin2 = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l, xmlin2 = "FieldInfoTest.not_xml")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        """
        Test output parameter,
        first,   test its default is none
        second,  test it raises an error when it isn't a string
        """
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l)
            self.assertEqual(self.Constructor.Output, None, "default is not None")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l, output = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        """
        Test mergein parameter,
        first,   test its default is Merge.xsl.in
        second,  test it raises an error when it isn't a string
        """
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l, mergein = "something else")
            self.assertEqual(self.Constructor.MergeIn, "Merge.xsl.in", "default is not Merge.xsl.in")
        except:
            pass
        
        try :
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l, mergin = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        """
        Test mergeout parameter,
        first,   test its default is Merge.xsl
        second,  test it raises an error when it isn't a string
        third,   test it raises an error when it isn't an xsl
        """
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l)
            self.assertEqual(self.Constructor.MergeOut, "Merge.xsl", "default is not Merge.xsl")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l, mergout = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = xml_in_1, xsl = x_s_l, mergeout = "MergedOut.not_xsl")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass


    def test_parse_xslt(self):
        """
        Test to check that the outputted file has got information within it
        """
        self.CADImport_two_xml.parse_xslt()
        OutputFile = self.CADImport_two_xml.output
        self.assertTrue( os.path.getsize(OutputFile) != 0)
        """
        Test to check that the information within the outputted file is of MAUS Module format
        """
        count = 0
        fin = open(OutputFile, 'r')
        for lines in fin.readlines():
            #line below needs to be reviewed as each file is different
            if lines.find('Configuration') >= 0 or lines.find('Dimensions') >= 0 or lines.find('Position') >= 0 or lines.find('Rotation') >= 0:
                count += 1
        if count == 0:
            self.assertTrue(False, "output file is not of MAUS Module format")
        os.remove(self.out_put)

    def test_append_merge(self):
        self.CADImport_two_xml.append_merge()
        """
        this test check to see if the word EDIT was replaced in Merge.xsl.in by
        checking the output file Merge.xsl for the word EDIT
        """
        count1 = 0
        fout = open(self.CADImport_two_xml.merge_out, 'r')
        for line in fout.readlines():
            if line.find('EDIT')>=0:
                count1 += 1
        self.assertTrue(count1 == 0, "EDIT was not replaced in Merge.xsl")
        """
        this tests to make sure there is only on EDIT in the Merge.xsl.in file
        """
        count2 = 0
        fin = open(self.CADImport_two_xml.merge_in, 'r')
        for line in fin.readlines():
            if line.find('EDIT')>=0:
                count2 += 1
        self.assertTrue(count2 == 1, "More than one EDIT found in Merge.xsl.in")
        os.remove(self.CADImport_two_xml.merge_out)
        
    def tearDown(self):
        self.CADImport_two_xml = None
        xml_in_1 = None
        xml_in_2 = None
        x_s_l = None
        out_put = None
        
    
if __name__ == '__main__':
    unittest.main()
