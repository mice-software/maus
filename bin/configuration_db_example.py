"""Configuration database demo macro
"""
#!/usr/bin/env python

#################################################################
###!!! YOU ARE NOT ALLOWED TO MODIFY THIS FILE DIRECTLY    !!!###
###!!! PLEASE MAKE A COPY OF THIS FILE WITH THE CP COMMAND !!!###
#################################################################

# pylint: disable=R0915

import pprint  #  pretty printer

from datetime import datetime  #  for getting runs over a time range
from cdb import Beamline       #  get beamline information for some run

def run():
    """Fetch information from the configuration database
    
    This script will show you how to fetch information from the configuration
    database.  It will run you through many of the steps so you can use this
    within your own macro.
    """

    # This is used to nicely print python dictionaries
    pretty_print = pprint.PrettyPrinter(indent=4)

    print '*********************************************************'
    print '** Welcome to the verbose configuration DB demo script **'
    print '*********************************************************'
    print

    print "First let's take a peak at what run information looks like for one"
    print "run.  Let's choose run 2873 since that's included within MAUS. We do"
    print 'this by first setting up the configuration DB interface:'
    print
    print '\tbeamline = Beamline()'
    beamline = Beamline()
    
    print
    print 'which was easy enough.  You can check that the beamline class was'
    print 'set up correctly by running:'
    print
    print '\tget_status()'
    print
    print 'which should return OK (returns: %s).' % beamline.get_status()
    print
    print "So now let's get something useful.  How about the setup our run:"
    print
    print '\tprint beamline.get_beamline_for_run(2873)'
    pretty_print.pprint(beamline.get_beamline_for_run(2873))

    print
    print "We can also ask what the runs were for a certain date range:"
    print
    print '\tdate0 = datetime.strptime("2009-01-01", "%Y-%m-%d")'
    print '\tdate1 = datetime.strptime("2010-12-12", "%Y-%m-%d")'
    print '\trun_info = beamline.get_beamlines_for_dates(date0, date1)'
    print '\tprint run_info.keys()'
    date0 = datetime.strptime("2010-08-01", "%Y-%m-%d")
    date1 = datetime.strptime("2010-08-12", "%Y-%m-%d")
    run_info = beamline.get_beamlines_for_dates(date0, date1)
    print run_info.keys()
    print
    print 'where the .keys() stuff only prints the run number since the data-'
    print 'type is a dictionary.'  

    print "Let's say you want the magnet current for a certain run:"
    print
    print "\tmy_runs = beamline.get_beamline_for_run(2873)"
    print "\tprint my_runs[2873]['magnets']['d1']['setCurrent']"
    my_runs = beamline.get_beamline_for_run(2873)
    print my_runs[2873]['magnets']['d1']['setCurrent']

    print 'Lastly you can search by target pulses:'
    print 
    print '\tget_beamlines_for_pulses(565665,565864).keys()'
    print beamline.get_beamlines_for_pulses(565665, 565864).keys()
    print
    print 'and you can find more information about the configuration DB'
    print 'interface at:'
    print
    print '${MAUS_ROOT_DIR}/src/core/cdb/_functional_tests.py'
    print


if __name__ == '__main__':
    run()
