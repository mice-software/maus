import test_CADImport
import unittest
from CADImport import CADImport

class  test_CADImport(unittest.TestCase):
    def setUp(self):
        self.CADImport_one_xml = CADImport.__init__("fastradModel.xml", "GDML2G4MICE.xsl")
        #self.CADImport_two_xml = CADImport.__init__("fastradModel.xml", "GDML2G4MICE.xsl", "FieldInfoTest.xml")

    def test_initialiser(self):
        try:
            CADImport.__init__()
            assertTrue(False, "Should have raised a StandardError")
        except StandardError:
            pass

    def test_XSLTParse(self):
        try:
            self.CADImport_one_xml.XSLTParse()
            self.assertTrue(False, "Should have raised a StandardError")
        except StandardError:
            pass
        except:
            self.assertTrue(False, "Should have raised a StandardError")

    def test_AppendMerge(self):
        try:
            self.CADImport_one_xml.AppendMerge()
            self.assertTrue(False, "Should have raised a StandardError")
        except StandardError:
            pass
        except:
            self.assertTrue(False, "Should have raised a StandardError")

    #def tearDown(self):
    #    self.foo.dispose()
    #    self.foo = None

if __name__ == '__main__':
    unittest.main()