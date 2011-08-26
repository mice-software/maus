"cdb test module."

# Disable reimport warning since we want to do that if this is the main function
# pylint: disable=W0404

import unittest
import sys

import test_geometry.test_CADImport
import test_geometry.test_ConfigReader
import test_geometry.test_GDMLFormatter
import test_geometry.test_GDMLPacker
import test_geometry.test_GDMLtoCDB
import test_geometry.test_GDMLtoMAUSModule


if __name__ == '__main__':
    SUITE1 = test_geometry.test_CADImport.suite()
    SUITE2 = test_geometry.test_ConfigReader.suite()
    SUITE3 = test_geometry.test_GDMLFormatter.suite()
    SUITE4 = test_geometry.test_GDMLPacker.suite()
    SUITE5 = test_geometry.test_GDMLtoCDB.suite()
    SUITE6 = test_geometry.test_GDMLtoMAUSModule.suite()

    SUITE = unittest.TestSuite()
    SUITE.addTest(SUITE1)
    SUITE.addTest(SUITE2)
    SUITE.addTest(SUITE3)
    SUITE.addTest(SUITE4)
    SUITE.addTest(SUITE5)
    SUITE.addTest(SUITE6)
    unittest.TextTestRunner(verbosity=2).run(SUITE)

