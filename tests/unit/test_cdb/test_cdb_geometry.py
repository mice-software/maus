"_geometry test module"

# pylint: disable=C0103,R0904

import unittest
from datetime import datetime
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
from cdb import Geometry
from cdb import CdbPermanentError
from cdb import CdbTemporaryError


class TestGeometry(unittest.TestCase):
    "Test the _geometry module."

    def setUp(self):
        "Set up data for use in tests."
        self._geo = Geometry()

    def test_str(self):
        "Test Geometry __str__."
        self.assertEquals(self._geo.__str__(), _constants.G_STR)

    def test_get_current_gdml(self):
        "Test Geometry get_current_gdml."
        self._geo.set_url("error")
        self.assertRaises(CdbPermanentError, self._geo.get_current_gdml)
        self._geo.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._geo.get_current_gdml)
        self._geo.set_url("ok")
        self.assertEquals(self._geo.get_current_gdml(),
                          _constants.G_GDML_CURRENT)

    def test_get_gdml_for_id(self):
        "Test Geometry get_gdml_for_id."
        _id = 1
        self._geo.set_url("error")
        self.assertRaises(CdbPermanentError, self._geo.get_gdml_for_id, _id)
        self._geo.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._geo.get_gdml_for_id, _id)
        self._geo.set_url("ok")
        self.assertEquals(self._geo.get_gdml_for_id(_id),
                          _constants.G_GDML_FOR_ID)

    def test_get_gdml_for_run(self):
        "Test Geometry get_gdml_for_run."
        _run = 1
        self._geo.set_url("error")
        self.assertRaises(CdbPermanentError, self._geo.get_gdml_for_run, _run)
        self._geo.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._geo.get_gdml_for_run, _run)
        self._geo.set_url("ok")
        self.assertEquals(self._geo.get_gdml_for_run(_run),
                          _constants.G_GDML_FOR_RUN)

    def test_get_ids(self):
        "Test Geometry get_ids."
        _start_time = datetime.strptime("1999-12-31 23:59:59.999999",
                                        "%Y-%m-%d %H:%M:%S.%f")
        _stop_time = datetime.strptime("2029-12-31 23:59:59.999999",
                                        "%Y-%m-%d %H:%M:%S.%f")
        self._geo.set_url("error")
        self.assertRaises(CdbPermanentError, self._geo.get_ids, _start_time)
        self._geo.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._geo.get_ids, _start_time)
        self._geo.set_url("ok")
        self.assertEquals(self._geo.get_ids(_start_time), _constants.G_IDS)


def suite():
    "Add tests to the suite."
    _suite = unittest.TestSuite()
    _suite.addTest(unittest.makeSuite(TestGeometry))
    return _suite

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())

