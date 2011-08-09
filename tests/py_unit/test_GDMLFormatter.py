import unittest
import shutil
import os
from geometry.GDMLFormatter import Formatter

class  test_GDMLFormatter(unittest.TestCase):
    def setUp(self):
        self.Constructor = None
        testCase = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testGeometry'
        self.GDML = Formatter(testCase)

    def test_Constructor(self):
        try:
            self.Constructor = Formatter(1)
            self.assertTrue(False, 'should have raised an exception')
        except:
            pass
        
        try:
            testCase = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testFormatter'
            self.Constructor = Formatter(testCase)
            self.assertTrue(False, 'should have raised an exception')
        except:
            pass
        
    def test_format(self):
        self.GDML.format()
        count = 0
        fin = open(self.GDML.configurationfile, 'r')
        for lines in fin.readlines():
            if lines.find(self.GDML.schema) >= 0:
                count += 1
            if lines.find(self.GDML.materialfile) >= 0:
                count += 1
            if lines.find('<!-- Formatted for MAUS -->') >= 0:
                count += 1
        self.assertEqual(3, count, 'Formatting not complete')
        count = 0
        fin.close()
        length = len(self.GDML.stepfiles)
        for num in range(0, length):
            fin = open(self.GDML.stepfiles[num], 'r')
            for lines in fin.readlines():
                if lines.find(self.GDML.schema) >= 0:
                    count += 1
                if lines.find(self.GDML.materialfile) >= 0:
                    count += 1
                if lines.find('<!-- Formatted for MAUS -->') >= 0:
                    count += 1
            self.assertEqual(3, count, 'Formatting not complete')
            count = 0
            fin.close()
               
    def tearDown(self):
        testcase = None

if __name__ == '__main__':
    unittest.main()