"These tests depend on a working Config DB."

# pylint: disable=C0103,R0801

from datetime import datetime

from cdb import CdbError
from cdb import AlarmHandler
from cdb import Beamline
from cdb import Control
from cdb import Geometry

def _alarm_handler_test():
    """ Test AlarmHandler methods """
    print '\nALARMHANDLER TESTS'
    print '~~~~~~~~~~~~~~~~~~'
    _ah = AlarmHandler()
    print _ah
    duff = False
    try:
        _ah.set_url("duff")
        print "ERROR - set_url should have thrown a CdbError"
        duff = True
    except CdbError:
        print '\nPassed duff URL test'
    if duff:
        raise CdbError("set_url should have thrown a CdbError")
    print '\nget_status()'
    print _ah.get_status()
    print '\nlist_tags()'
    print _ah.list_tags()
    print '\nget_tagged_alh("Harry")'
    print _ah.get_tagged_alh("Harry")
    print '\nget_used_alh("2011-02-12 23:59:59.0")'
    print _ah.get_used_alh(datetime.strptime("2011-02-12 23:59:59.0",
                                             "%Y-%m-%d %H:%M:%S.%f"))
    print '\nget_used_tags("2011-02-09 08:56:18.5", "2011-12-12 23:59:59.0")'
    print _ah.get_used_tags(datetime.strptime("2011-02-09 08:56:18.5",
                                              "%Y-%m-%d %H:%M:%S.%f"),
                            datetime.strptime("2011-12-12 23:59:59.0",
                                               "%Y-%m-%d %H:%M:%S.%f"))
    return

def _beamline_test():
    """ Test Beamline methods """
    print '\nBEAMLINE TESTS'
    print '~~~~~~~~~~~~~~'
    _bl = Beamline()
    print _bl
    duff = False
    try:
        _bl.set_url("duff")
        print "ERROR - set_url should have thrown a CdbError"
        duff = True
    except CdbError:
        print '\nPassed duff URL test'
    if duff:
        raise CdbError("set_url should have thrown a CdbError")
    print '\nget_status()'
    print _bl.get_status()
    print '\nget_beamline_for_run(2879)'
    print _bl.get_beamline_for_run(2879)
    print '\nget_beamlines_for_pulses(565665,565864)'
    _runs = _bl.get_beamlines_for_pulses(565665, 565864)
    print "beamlines returned:" + str(len(_runs))
    print _runs
    try:
        _runs = _bl.get_beamlines_for_dates("2009-01-01 10:23:00",
                                     "2010-12-12 23:59:59")
        print "ERROR - get_beamlines_for_dates should have thrown a CdbError"
        duff = True
    except CdbError:
        print '\nPassed duff timestamp test'
    if duff:
        raise CdbError("get_beamlines_for_dates should have thrown a CdbError")
    print ('\nget_beamlines_for_dates("2009-01-01 10:23:00.5", "2010-12-12 '
        '23:59:59") - only printing data for first run in dictionary')    
    _runs = _bl.get_beamlines_for_dates(
        datetime.strptime("2009-01-01 10:23:00.5", "%Y-%m-%d %H:%M:%S.%f"),
        datetime.strptime("2010-12-12 23:59:59", "%Y-%m-%d %H:%M:%S"))
    print "beamlines returned:" + str(len(_runs))
    for k, value in _runs.iteritems():
        print k, value
        break
    print ('\nget_beamlines_for_dates("2009-01-01 10:23:00.5", None)')
    _runs = _bl.get_beamlines_for_dates(
        datetime.strptime("2009-01-01 10:23:00.5", "%Y-%m-%d %H:%M:%S.%f"),
        None)
    print "beamlines returned:" + str(len(_runs))
    print _runs
    print ('\nget_beamlines_for_dates("2009-01-01 10:23:00.5")')    
    _runs = _bl.get_beamlines_for_dates(
        datetime.strptime("2009-01-01 10:23:00.5", "%Y-%m-%d %H:%M:%S.%f"))
    print "beamlines returned:" + str(len(_runs))
    print _runs
    print ('\nget_all_beamlines() - '
        'only printing data for first run in dictionary')
    _runs = _bl.get_all_beamlines()
    print "beamlines returned:" + str(len(_runs))
    for k, value in _runs.iteritems():
        print k, value
        break
    print '\nget_beamline_for_run(999999)'
    print _bl.get_beamline_for_run(999999,)
    return

def _control_test():
    """ Test Control methods """
    print '\nCONTROL TESTS'
    print '~~~~~~~~~~~~~'
    _control = Control()
    print _control
    duff = False
    try:
        _control.set_url("duff")
        print "ERROR - set_url should have thrown a CdbError"
        duff = True
    except CdbError:
        print '\nPassed duff URL test'
    if duff:
        raise CdbError("set_url should have thrown a CdbError")
    print '\nget_status()'
    print _control.get_status()
    print '\ngetControls()'
    print _control.get_controls()
    print '\ngetControlsForCrate(2)'
    print _control.get_controls_for_crate(2)
#    print '\ngetPreviousSettings("2010-12-12 23:59:59")'
#    print _control.get_previous_settings(
#        datetime.strptime("2010-12-12 23:59:59", "%Y-%m-%d %H:%M:%S"))
    return

def _geometry_test():
    """ Test Geometry methods """
    print '\nGEOMETRY TESTS'
    print '~~~~~~~~~~~~~~'
    _geometry = Geometry()
    print _geometry
    duff = False
    try:
        _geometry.set_url("duff")
        print "ERROR - set_url should have thrown a CdbError"
        duff = True
    except CdbError:
        print '\nPassed duff URL test'
    if duff:
        raise CdbError("set_url should have thrown a CdbError")
    print '\nget_status()'
    print _geometry.get_status()
    print '\nget_current_gdml()'
    print 'GDML length:' + str(len(_geometry.get_current_gdml()))
    print '\nget_gdml_for_id(9)'
    print 'GDML length:' + str(len(_geometry.get_gdml_for_id(9)))
    print '\nget_gdml_for_run(1)'
    print 'GDML length:' + str(len(_geometry.get_gdml_for_run(1)))
    try:
        print _geometry.get_ids("2010-12-12 23:59:59", "2010-12-12 23:59:59")
        print "ERROR - get_ids should have thrown a CdbError"
        duff = True
    except CdbError:
        print '\nPassed duff timestamp test'
    if duff:
        raise CdbError("get_ids should have thrown a CdbError")
    print '\nget_ids("2010-12-12 23:59:58.5", "2010-12-12 23:59:59")'
    print _geometry.get_ids(datetime.strptime("2010-12-12 23:59:58.5",
                                              "%Y-%m-%d %H:%M:%S.%f"),
                            datetime.strptime("2010-12-12 23:59:59",
                                              "%Y-%m-%d %H:%M:%S"))

    print '\nget_ids("2010-12-12 23:59:58.5", None)'
    print _geometry.get_ids(datetime.strptime("2010-12-12 23:59:58.5",
                                               "%Y-%m-%d %H:%M:%S.%f"), None)
    print '\nget_ids("2010-12-12 23:59:58.5")'
    print _geometry.get_ids(datetime.strptime("2010-12-12 23:59:58.5",
                                               "%Y-%m-%d %H:%M:%S.%f"))
    return

def _test():
    """ Runs the tests for each class. """
    message = ""
    try:
        _alarm_handler_test()
    except CdbError, error:
        message = message + "ERROR while running alarm_handler tests\n"
        message = message + str(error) + "\n"
    try:
        _beamline_test()
    except CdbError, error:
        message = message + "ERROR while running beamline tests\n"
        message = message + str(error) + "\n"
    try:
        _control_test()
    except CdbError, error:
        message = message + "ERROR while running control tests\n"
        message = message + str(error) + "\n"
    try:
        _geometry_test()
    except CdbError, error:
        message = message + "ERROR while running geometry tests\n"
        message = message + str(error) + "\n"

    if message:
        print "\n\nERROR running tests"
        print "~~~~~~~~~~~~~~~~~~~\n"
        print message
        return
    print "\nAll tests passed"
    return

if __name__ == "__main__":
    _test()

