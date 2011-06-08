"__init__ test module"

# pylint: disable=W0404,W0612,R0904
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

from test_cdb import _constants


class TestInit(unittest.TestCase):
    "Test the __init__ module."

    def test_imports(self):
        "Test __init__ imports."
        from cdb import CdbError
        from cdb import CdbPermanentError
        from cdb import CdbTemporaryError
        from cdb import AlarmHandler
        from cdb import Beamline
        from cdb import Control
        from cdb import Geometry
        self.assertEqual(dir(), _constants.CLASS_LIST)

def suite():
    "Add tests to the suite."
    _suite = unittest.TestSuite()
    _suite.addTest(unittest.makeSuite(TestInit))
    return _suite

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())

