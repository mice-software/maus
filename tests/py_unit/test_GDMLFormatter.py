import unittest
import shutil
import os
from geometry.GDMLFormatter import formatter

class  test_GDMLFormatter(unittest.TestCase):
    def setUp(self):
        self.Constructor = None
        testCase = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testFormatter'
        self.GDML = formatter(testCase)

    def test_Constructor(self):
        try:
            self.Constructor = formatter(1)
            self.assertTrue(False, 'should have raised an exception')
        except:
            pass
        
    def test_test_GDMLFormatter(self):
        self.GDML.format()
        fout = os.environ['MAUS_ROOT_DIR'] + '/src/common_py/geometry/testCases/testFormatter'
        shutil.copy(self.GDML.ConfigurationFile, fout)
        shutil.copy(self.GDML.MaterialFile, fout)
        length = len(self.GDML.StepFiles)
        for num in range(0, length):
            shutil.copy(self.GDML.StepFiles[num], fout)

    #def tearDown(self):
    #    self.foo.dispose()
    #    self.foo = None

if __name__ == '__main__':
    unittest.main()