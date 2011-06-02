"_control test module"

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
from cdb import Control
from cdb import CdbPermanentError
from cdb import CdbTemporaryError


class TestControl(unittest.TestCase):
    "Test the _control module."

    def setUp(self):
        "Set up data for use in tests."
        self._control = Control()

    def test_str(self):
        "Test Control __str__."
        self.assertEquals(self._control.__str__(), _constants.C_STR)

    def test_get_controls(self):
        "Test Control get_controls"
        self._control.set_url("error")
        self.assertRaises(CdbPermanentError, self._control.get_controls)
        self._control.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._control.get_controls)
        self._control.set_url("ok")
        self.assertEquals(self._control.get_controls(),
                          _constants.C_CONTROLS)

    def test_get_controls_for_crate(self):
        "Test Control get_controls_for_crate."
        _crate = 1
        self._control.set_url("error")
        self.assertRaises(CdbPermanentError,
                          self._control.get_controls_for_crate, _crate)
        self._control.set_url("warn")
        self.assertRaises(CdbTemporaryError,
                          self._control.get_controls_for_crate, _crate)
        self._control.set_url("ok")
        self.assertEquals(self._control.get_controls_for_crate(_crate),
                          _constants.C_CONTROLS_FOR_CRATE)

    def test_get_previous_settings(self):
        "Test Control get_previous_settings."
        _timestamp = datetime.strptime("1999-12-31 23:59:59.999999",
                                "%Y-%m-%d %H:%M:%S.%f")
        self._control.set_url("error")
        self.assertRaises(CdbPermanentError,
                          self._control.get_previous_settings, _timestamp)
        self._control.set_url("warn")
        self.assertRaises(CdbTemporaryError,
                          self._control.get_previous_settings, _timestamp)
        self._control.set_url("ok")
        self.assertEquals(self._control.get_previous_settings(_timestamp),
                          _constants.C_CONTROLS_FOR_PREVIOUS)


def suite():
    "Add tests to the suite."
    _suite = unittest.TestSuite()
    _suite.addTest(unittest.makeSuite(TestControl))
    return _suite

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())

