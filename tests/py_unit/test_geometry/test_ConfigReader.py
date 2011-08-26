import os
import unittest
from geometry.ConfigReader import Configreader

class  test_ConfigReader(unittest.TestCase):
    def setUp(self):
        self.testcase = Configreader()
        
    def test_readconfig(self):
        self.testcase.readconfig()
        gdml_dir = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry'
        self.assertEqual(self.testcase.gdmldir, gdml_dir, 'GDML Directory not found, testConfigReader::test_readconfig')
        test_note = 'This is a standard note. It uses the standard test case.'
        self.assertEqual(self.testcase.geometrynotes, test_note, 'Note not found, testConfigReader::test_readconfig')
        self.assertEqual(self.testcase.zipfile, 0, 'Zip file arg not found, testConfigReader::test_readconfig')
        self.assertEqual(self.testcase.deleteoriginals, 0, 'Delete originals arg not found, testConfigReader::test_readconfig')
        test_download = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/Download/'
        self.assertEqual(self.testcase.downloaddir, test_download, 'Download directory not found, testConfigReader::test_readconfig')
        self.assertEqual(self.testcase.starttime, '2011-08-25 09:00:00', 'Start time not found, testConfigReader::test_readconfig')
        self.assertEqual(self.testcase.stoptime, None, 'Start time not found, testConfigReader::test_readconfig')

    def tearDown(self):
        self.testcase = None

if __name__ == '__main__':
    unittest.main()
