import unittest
from GDMLFormatter import formatter

class  test_GDMLFormatter(unittest.TestCase):
    def setUp(self):
        self.Constructor = None
        self.GDML = formatter('/home/matt/maus_littlefield/src/common_py/geometry/testCases/testFormatter')

    def test_Constructor(self):
        try:
            self.Constructor = formatter(1)
            self.assertTrue(False, 'should have raised an exception')
        except:
            pass
        
    def test_test_GDMLFormatter(self):
        self.GDML.format()
        fin = open(self.GDML.ConfigurationFile)

    #def tearDown(self):
    #    self.foo.dispose()
    #    self.foo = None

if __name__ == '__main__':
    unittest.main()

