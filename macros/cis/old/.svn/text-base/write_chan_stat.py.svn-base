# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Write channel status information related to CIS
#
# March 10, 2009
#

execfile('src/load.py', globals()) # don't remove this!


"""
Step 1:
You need to set the 'runs' variable to reflect which runs you would like to look at.  There are various examples below.
"""

runs = '-1 month' # Use the last month of runs
#runs = [101184]  # Just use one run, useful for debugging
        
# This is the worker processing loop.
Go([\
    Use(run=runs, runType='CIS'),                      # Tell Tucs which runs to use                       \
    ReadCISFile(),                                     # Tell Tucs to load in the CIS information          \
    ReadDB(),                                          # Read CIS constants from DB as well                \
    CalibBadBit(),                                     # Set the 'isCalibrated' flag for each channel      \
    CopyLocalChanStat(),                               # Make a copy of the DB                             \
    WriteChanStat(criterion='isCalibrated_qflag',      # Write out the channel status information          \
                  run=102540),            \
    IsEntireModuleBad(),  \
    ])


