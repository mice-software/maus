import os.path
import unittest
from geometry.CADImport import CADImport

class  test_CADImport(unittest.TestCase):
    def setUp(self):
        self.Constructor = None
        self.CADImport_two_xml = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl", xmlin2 = "FieldInfoTest.xml", output = "OUTPUTFILE.txt")

    def test_Constructor(self):
        """
        Test xmlin1 parameter,
        first,   test it raises an error when there is no input
        second,  test it raises an error when it isn't an xml or gdml file
        """
        try:
            self.Constructor = CADImport(xsl = "GDML2G4MICE.xsl")
            self.assertTrue(False, "should have raised an exception")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.not_xml", xsl = "GDML2G4MICE.xsl")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        """
        Test xslin1 parameter,
        first,   test it raises an error when there is no input
        second,  test it raises an error when it isn't an xsl file
        """
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.not_xsl")
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
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl")
            self.assertEqual(self.Constructor.XMLIn2, None, "default is not None")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl", xmlin2 = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl", xmlin2 = "FieldInfoTest.not_xml")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        """
        Test output parameter,
        first,   test its default is none
        second,  test it raises an error when it isn't a string
        """
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl")
            self.assertEqual(self.Constructor.Output, None, "default is not None")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl", output = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        """
        Test mergein parameter,
        first,   test its default is Merge.xsl.in
        second,  test it raises an error when it isn't a string
        """
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl", mergein = "something else")
            self.assertEqual(self.Constructor.MergeIn, "Merge.xsl.in", "default is not Merge.xsl.in")
        except:
            pass
        
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl", mergin = 1)
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
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl")
            self.assertEqual(self.Constructor.MergeOut, "Merge.xsl", "default is not Merge.xsl")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl", mergout = 1)
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass
        try:
            self.Constructor = CADImport(xmlin1 = "fastradModel.xml", xsl = "GDML2G4MICE.xsl", mergeout = "MergedOut.not_xsl")
            self.assertTrue(False, "Should have raised an exception")
        except:
            pass


    def test_XSLTParse(self):
        """
        Test to check that the outputted file has got information within it
        """
        self.CADImport_two_xml.XSLTParse()
        OutputFile = self.CADImport_two_xml.Output
        self.assertTrue( os.path.getsize(OutputFile) != 0)
        """
        Test to check that the information within the outputted file is of MAUS Module format
        """
        count = 0
        fin = open(OutputFile, 'r')
        for lines in fin.readlines():
            if lines.find('Configuration') >= 0 or lines.find('Dimensions') >= 0 or lines.find('Position') >= 0 or lines.find('Rotation') >= 0:
                count += 1
        if count == 0:
            self.assertTrue(False, "output file is not of MAUS Module format")

    def test_AppendMerge(self):
        self.CADImport_two_xml.AppendMerge()
        """
        this test check to see if the word EDIT was replaced in Merge.xsl.in by
        checking the output file Merge.xsl for the word EDIT
        """
        count1 = 0
        fout = open(self.CADImport_two_xml.MergeOut, 'r')
        for line in fout.readlines():
            if line.find('EDIT')>=0:
                count1 += 1
        self.assertTrue(count1 == 0, "EDIT was not replaced in Merge.xsl")
        """
        this tests to make sure there is only on EDIT in the Merge.xsl.in file
        """
        count2 = 0
        fin = open(self.CADImport_two_xml.MergeIn, 'r')
        for line in fin.readlines():
            if line.find('EDIT')>=0:
                count2 += 1
        self.assertTrue(count2 == 1, "More than one EDIT found in Merge.xsl.in")
        
    def tearDown(self):
        self.CADImport_two_xml = None
    
if __name__ == '__main__':
    unittest.main()
