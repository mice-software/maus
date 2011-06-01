"_alarmhandler test module"

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
from cdb import AlarmHandler
from cdb import CdbPermanentError
from cdb import CdbTemporaryError


class TestAlarmHandler(unittest.TestCase):
    "Test the _alarmhandler module."

    def setUp(self):
        "Set up data for use in tests."
        self._alh = AlarmHandler()

    def test_str(self):
        "Test AlarmHandler __str__."
        self.assertEquals(self._alh.__str__(), _constants.ALH_STR)

    def test_get_tagged_alh(self):
        "Test AlarmHandler get_tagged_alh."
        _tag = "spam"
        self._alh.set_url("error")
        self.assertRaises(CdbPermanentError, self._alh.get_tagged_alh, _tag)
        self._alh.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._alh.get_tagged_alh, _tag)
        self._alh.set_url("ok")
        self.assertEquals(self._alh.get_tagged_alh(_tag),
                          _constants.ALH_TAGGED_ALH)

    def test_get_used_alh(self):
        "Test AlarmHandler get_used_alh."
        _timestamp = datetime.strptime("1999-12-31 23:59:59.999999",
                                        "%Y-%m-%d %H:%M:%S.%f")
        self._alh.set_url("error")
        self.assertRaises(CdbPermanentError, self._alh.get_used_alh, _timestamp)
        self._alh.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._alh.get_used_alh, _timestamp)
        self._alh.set_url("ok")
        self.assertEquals(self._alh.get_used_alh(_timestamp),
                          _constants.ALH_USED_ALH)

    def test_get_used_tags(self):
        "Test AlarmHandler get_used_tags."
        _start_time = datetime.strptime("1999-12-31 23:59:59.999999",
                                        "%Y-%m-%d %H:%M:%S.%f")
        _stop_time = datetime.strptime("2029-12-31 23:59:59.999999",
                                        "%Y-%m-%d %H:%M:%S.%f")
        self._alh.set_url("error")
        self.assertRaises(CdbPermanentError, self._alh.get_used_tags,
                          _start_time)
        self._alh.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._alh.get_used_tags,
                          _start_time)
        self._alh.set_url("ok")
        self.assertEquals(self._alh.get_used_tags(_start_time, _stop_time),
                          _constants.ALH_USED_TAGS)

    def test_list_tags(self):
        "Test AlarmHandler list_tags."
        self._alh.set_url("error")
        self.assertRaises(CdbPermanentError, self._alh.list_tags)
        self._alh.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._alh.list_tags)
        self._alh.set_url("ok")
        self.assertEquals(self._alh.list_tags(), _constants.ALH_LIST_TAGS)


def suite():
    "Add tests to the suite."
    _suite = unittest.TestSuite()
    _suite.addTest(unittest.makeSuite(TestAlarmHandler))
    return _suite

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())

