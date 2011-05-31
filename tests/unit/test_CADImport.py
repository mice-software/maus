import os.path
import unittest
from CADImport import CADImport

class  test_CADImport(unittest.TestCase):
    def setUp(self):
        self.CADImport_one_xml = CADImport.__init__("fastradModel.xml", "GDML2G4MICE.xsl")
        self.CAD_Import_two_xml = CADImport("fastradModel.xml", "GDML2G4MICE.xsl", "FieldInfoTest.xml", "~/maus_littlefield/tmp/OUTPUTFILE.txt")

    def test_XSLTParse(self):
        try:
            self.CAD_Import_two_xml.XSLTParse()
            OutputFile = test1.Output
            if os.path.getsize(OutputFile)== 0: raise StandardError("No output file produced")
        except StandardError:
            pass
        except:
            self.assertTrue(False, "Should have raised a StandardError")

    def test_AppendMerge(self):
        self.CAD_Import_two_xml.AppendMerge()
        try:
            count1 = 0
            fout = open(self.CAD_Import_two_xml.MergeOut, 'r')
            for line in fout.readlines():
                if line.find('EDIT')>=0: count1 = count1 + 1
            if count1 != 0: raise StandardError("EDIT was not replaced")

            count2 = 0
            fin = open(self.CAD_Import_two_xml.MergeIn, 'r')
            for line in fin.readlines():
                if line.find('EDIT')>=0: count2 = count2 + 1
            if count2 != 1: raise StandardError("Wihin Merge.xsl, there is no EDIT or there are too many")
        except StandardError:
            pass
        except:
            self.assertTrue(False, "Should have raised a StandardError")

    #def tearDown(self):
    #    self.foo.dispose()
    #    self.foo = None

if __name__ == '__main__':
    unittest.main()
