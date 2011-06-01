"cdb test module."

# Disable reimport warning since we want to do that if this is the main function
# pylint: disable=W0404

import unittest
import sys

# This block is repeated in each test and helps the test suite ensure that suds
# is properly mocked by the test version.
FOUND_SUDS = None
for x in sys.path:
    if 'suds' in x:
        FOUND_SUDS = x
if FOUND_SUDS:
    sys.path.remove(FOUND_SUDS)

import test_cdb.test_cdb__init__
import test_cdb.test_cdb_alarmhandler
import test_cdb.test_cdb_base
import test_cdb.test_cdb_beamline
import test_cdb.test_cdb_control
import test_cdb.test_cdb_geometry


if __name__ == '__main__':
    SUITE1 = test_cdb.test_cdb__init__.suite()
    SUITE2 = test_cdb.test_cdb_base.suite()
    SUITE3 = test_cdb.test_cdb_alarmhandler.suite()
    SUITE4 = test_cdb.test_cdb_beamline.suite()
    SUITE5 = test_cdb.test_cdb_control.suite()
    SUITE6 = test_cdb.test_cdb_geometry.suite()

    SUITE = unittest.TestSuite()
    SUITE.addTest(SUITE1)
    SUITE.addTest(SUITE2)
    SUITE.addTest(SUITE3)
    SUITE.addTest(SUITE4)
    SUITE.addTest(SUITE5)
    SUITE.addTest(SUITE6)
    unittest.TextTestRunner(verbosity=2).run(SUITE)

