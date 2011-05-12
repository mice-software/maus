# To change this template, choose Tools | Templates
# and open the template in the editor.

import unittest
import mausconfig

class  test_mausconfig(unittest.TestCase):
    def setUp(self):
        self.mausconfig_one_xml = mausconfig.Config('fastradModel.xml', 'GDML2G4MICE.xsl')
        self.mausconfig_two_xml = mausconfig.Config('fastradModel.xml', 'GDML2G4MICE.xsl', 'fieldtesting.xml')
    

    #def tearDown(self):
    #    self.foo.dispose()
    #    self.foo = None

    def test_AppendMerge(self):
        #assert x != y;
        #self.assertEqual(x, y, "Msg");
        try:
            self.mausconfig_one_xml.AppendMerge()
            self.assertTrue(False, 'Should have raised a TypeError')
        except TypeError:
            pass
        except:
            self.assertTrue(False, 'Should have raised a TypeError')

if __name__ == '__main__':
    unittest.main()

