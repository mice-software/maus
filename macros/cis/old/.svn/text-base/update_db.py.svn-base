# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This macro looks at channels that aren't calibrated and updates their
# CIS constant in a local tileSqlite.db database
#
# March 05, 2009
#
execfile('src/load.py', globals()) # don't remove this!

"""
Step 1:
You need to set the 'runs' variable to reflect which runs you would like to look at.  There are various examples below.
"""

runs = '-1 month' # Use the last month of runs
#runs = '-2 month'
#runs = [101184]  # Just use one run, useful for debugging

# This is the worker processing loop.
Go([\
        Use(run=runs, runType='CIS'),             # Tell Tucs which runs to use                       \
        ReadCISFile(),                            # Tell Tucs to load in the CIS information          \
        ReadDB(),                                 # Read CIS constants from DB as well                \
        CalibBadBit(),                            # Set the 'isCalibrated' flag for each channel      \
        TimeStability(all=True, savePlot=False),  # Compute the calibration RMS                       \
        CopyDBConstants('CIS', recreate=True),    # Make a copy of the DB                             \
        IsDefaultCalib(),                         # Determine which channels have the default calib.  \
        ReadRepairList(),                         # Read the list of replaced ADCs                    \
        IsStable(),                               # Determine which channels are stable in time       \
        WriteDB(),                                # Write out the channel status information          \
        ])
