"_beamline test module."

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
from cdb import Beamline
from cdb import CdbPermanentError
from cdb import CdbTemporaryError


class TestBeamline(unittest.TestCase):
    "Test the _beamline module."

    def setUp(self):
        "Set up data for use in tests."
        self._bl = Beamline()

    def test_str(self):
        "Test Beamline __str__."
        self.assertEquals(self._bl.__str__(), _constants.BL_STR)

    def test_get_all_beamlines(self):
        "Test Beamline get_all_beamlines."
        self._bl.set_url("error")
        self.assertRaises(CdbPermanentError, self._bl.get_all_beamlines)
        self._bl.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._bl.get_all_beamlines)
        self._bl.set_url("ok")
        self.assertEquals(self._bl.get_all_beamlines(), _constants.BL_RUNS)

    def test_get_beamline_for_run(self):
        "Test Beamline get_beamline_for_run."
        _run_number = 666
        self._bl.set_url("error")
        self.assertRaises(CdbPermanentError, self._bl.get_beamline_for_run,
                          _run_number)
        self._bl.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._bl.get_beamline_for_run,
                          _run_number)
        self._bl.set_url("ok")
        self.assertEquals(self._bl.get_beamline_for_run(_run_number),
                          _constants.BL_RUN)

    def test_get_beamlines_for_dates(self):
        "Test Beamline get_beamlines_for_dates."
        _start_time = datetime.strptime("1999-12-31 23:59:59.999999",
                                        "%Y-%m-%d %H:%M:%S.%f")
        _stop_time = datetime.strptime("2029-12-31 23:59:59.999999",
                                        "%Y-%m-%d %H:%M:%S.%f")
        self._bl.set_url("error")
        self.assertRaises(CdbPermanentError, self._bl.get_beamlines_for_dates,
                          _start_time)
        self._bl.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._bl.get_beamlines_for_dates,
                          _start_time)
        self._bl.set_url("ok")
        self.assertEquals(self._bl.get_beamlines_for_dates(_start_time),
                          _constants.BL_DATES)
        self.assertEquals(self._bl.get_beamlines_for_dates(_start_time,
                          _stop_time), _constants.BL_DATES)
        self.assertEquals(self._bl.get_beamlines_for_dates
                         (_start_time, None), _constants.BL_DATES)

    def test_get_beamlines_for_pulses(self):
        "Test Beamline get_beamlines_for_pulses."
        _start_pluse = 1
        _end_pulse = 2
        self._bl.set_url("error")
        self.assertRaises(CdbPermanentError, self._bl.get_beamlines_for_pulses,
                          _start_pluse, _end_pulse)
        self._bl.set_url("warn")
        self.assertRaises(CdbTemporaryError, self._bl.get_beamlines_for_pulses,
                          _start_pluse, _end_pulse)
        self._bl.set_url("ok")
        self.assertEquals(self._bl.get_beamlines_for_pulses(_start_pluse,
                          _end_pulse), _constants.BL_PULSES)

def suite():
    "Add tests to the suite."
    _suite = unittest.TestSuite()
    _suite.addTest(unittest.makeSuite(TestBeamline))
    return _suite

if __name__ == '__main__':
    unittest.TextTestRunner(verbosity=2).run(suite())

