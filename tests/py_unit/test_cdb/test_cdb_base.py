"_base test module."

# pylint: disable=R0904

import sys

# This block is repeated in each test and helps the test suite ensure that suds
# is properly mocked by the test version.  
FOUND_SUDS = None
for x in sys.path:
    if 'suds' in x:
        FOUND_SUDS = x
if FOUND_SUDS:
    sys.path.remove(FOUND_SUDS)

import unittest
from datetime import datetime

from cdb import CdbPermanentError
from cdb import CdbTemporaryError
from cdb import CdbError
from cdb._base import _CdbBase
from cdb._base import _get_date_from_string
from cdb._base import _get_string_from_date

class TestBase(unittest.TestCase):
    "Test the _base module."

    def test_init(self):
        "Test _CdbBase __init__."
        self.assertRaises(CdbTemporaryError, _CdbBase, "URLError")
        self.assertRaises(CdbTemporaryError, _CdbBase, "TransportError")
        self.assertRaises(CdbPermanentError, _CdbBase, "ValueError")
        self.assertRaises(CdbPermanentError, _CdbBase, "InvalidURL")
        self.assertTrue(_CdbBase("ok"))

    def test_set_url(self):
        "Test _CdbBase set_url."
        _cdb = _CdbBase("ok")
        self.assertRaises(CdbTemporaryError, _cdb.set_url, "URLError")
        self.assertRaises(CdbTemporaryError, _cdb.set_url, "TransportError")
        self.assertRaises(CdbPermanentError, _cdb.set_url, "ValueError")
        self.assertRaises(CdbPermanentError, _cdb.set_url, "InvalidURL")

    def test_get_status(self):
        "Test _CdbBase get_status."
        _cdb = _CdbBase("ok")
        self.assertEquals(_cdb.get_status(), "feeling good")
        _cdb = _CdbBase("warn")
        self.assertRaises(CdbTemporaryError, _cdb.get_status)
        _cdb = _CdbBase("error")
        self.assertRaises(CdbPermanentError, _cdb.get_status)

    def test_get_status_exception_mesg(self):
        "Test _StatusHandler."
        #These tests also test the _exceptions module.
        _cdb = _CdbBase("warn")
        try:
            _cdb.get_status()
        except CdbTemporaryError, _execption:
            self.assertEquals(_execption.get_message(), "a bit poorly")
        try:
            _cdb.get_status()
        except CdbError, _execption:
            self.assertEquals(_execption.get_message(), "a bit poorly")
        _cdb = _CdbBase("error")
        try:
            _cdb.get_status()
        except CdbPermanentError, _execption:
            self.assertEquals(_execption.get_message(), "dead")
        try:
            _cdb.get_status()
        except CdbError, _execption:
            self.assertEquals(_execption.get_message(), "dead")

    def test_get_date_from_string(self):
        "Test _base _get_date_from_string."
        _dts = "1999-12-31 23:59:59"
        _dt = datetime.strptime(_dts, "%Y-%m-%d %H:%M:%S")
        self.assertEquals(_get_date_from_string(_dts), _dt)
        _dts = "1999-12-31 23:59:59.999999"
        _dt = datetime.strptime(_dts, "%Y-%m-%d %H:%M:%S.%f")
        self.assertEquals(_get_date_from_string(_dts), _dt)

    def test_get_string_from_date(self):
        "Test _base _get_string_from_date."
        _dts = "1999-12-31 23:59:59"
        _dt = datetime.strptime(_dts, "%Y-%m-%d %H:%M:%S")
        self.assertEquals(_get_string_from_date(_dt), _dts + ".000000")
        _dts = "1999-12-31 23:59:59.999999"
        _dt = datetime.strptime(_dts, "%Y-%m-%d %H:%M:%S.%f")
        self.assertEquals(_get_string_from_date(_dt), _dts)
        self.assertRaises(CdbPermanentError, _get_string_from_date,
                          "1999-13-31 23:59:59")


def suite():
    "Add tests to the suite."
    _suite = unittest.TestSuite()
    _suite.addTest(unittest.makeSuite(TestBase))
    return _suite

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())

