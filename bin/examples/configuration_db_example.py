#!/usr/bin/env python

"""Configuration database demo macro
"""

# pylint: disable=R0915

import sys #  system calls
import pprint  #  pretty printer
from datetime import datetime  #  for getting runs over a time range
import cdb # configuration database lookups
from cdb import Beamline       #  get beamline information for some run

def run():
    """Fetch information from the configuration database

    This script will show you how to fetch information from the configuration
    database.  It will run you through many of the steps so you can use this
    within your own macro.
    """

    # This is used to nicely print python dictionaries
    pretty_print = pprint.PrettyPrinter(indent=4)

    print """
    *********************************************************
    ** Welcome to the verbose configuration DB demo script **
    *********************************************************
    First let's take a peak at what run information looks like for one
    run.  Let's choose run 2873 since that's included within MAUS. We do
    this by first setting up the configuration DB interface:

    \tbeamline = Beamline()
    """
    try:
        beamline = Beamline()

        print """
        which was easy enough.  You can check that the beamline class was
        set up correctly by running:

            get_status()'

        which should return OK (returns: %s).' % beamline.get_status()

        So now let's get something useful.  How about the setup our run:

            print beamline.get_beamline_for_run(2873)
        """
        pretty_print.pprint(beamline.get_beamline_for_run(2873))

        print """
        We can also ask what the runs were for a certain date range:

            date0 = datetime.strptime("2009-01-01", "%Y-%m-%d")
            date1 = datetime.strptime("2010-12-12", "%Y-%m-%d")
            run_info = beamline.get_beamlines_for_dates(date0, date1)
            print run_info.keys()
        """
        date0 = datetime.strptime("2010-08-01", "%Y-%m-%d")
        date1 = datetime.strptime("2010-08-12", "%Y-%m-%d")
        run_info = beamline.get_beamlines_for_dates(date0, date1)
        print run_info.keys()
        print """
        where the .keys() stuff only prints the run number since the data-
        type is a dictionary.

        Let's say you want the magnet current for a certain run:"

            my_runs = beamline.get_beamline_for_run(2873)"
            print my_runs[2873]['magnets']['d1']['set_current']"
        """
        my_runs = beamline.get_beamline_for_run(2873)
        print  my_runs[2873]['magnets']['d1']['set_current']

        print """
        Lastly you can search by target pulses:'

            get_beamlines_for_pulses(565665,565864).keys()
        """
        print beamline.get_beamlines_for_pulses(565665, 565864).keys()
        print """
        and you can find more information about the configuration DB'
        interface by checking the interactive help on the python
        command line

            help(cdb)
        """
    except cdb._exceptions.CdbTemporaryError:
        sys.excepthook(*sys.exc_info())
        print """
        Failed to run example - check network connection!
        """

if __name__ == '__main__':
    run()
