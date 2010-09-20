# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This is a script intended to provide summary status information about the
# CIS system to somebody like the DQ leader.
#
# March 04, 2009
#

execfile('src/load.py', globals()) # don't remove this!


"""
Step 1:
You need to set the 'runs' variable to reflect which runs you would like to look at.  There are various examples below: the set of runs used for the Feb. 2009 TileWeek, the set of runs of Feb. 27th 2009, and the last example uses the last month of CIS runs.
"""

#runs = [100489,100699,100814,101152,101441,102009] # Runs as of Feb. 2009 TileWeek
#runs = [101184, 101441, 101784, 101785, 102009] # Runs as of Feb. 27, 2009
runs = '-1 month' # Use the last month of runs
#runs = [101184]  # Just use one run, useful for debugging
#runs = [102208, 103347, 101184, 102208, 102217, 102501, 102893, 103322, 103347]
#runs = [103347]
#runs = [103864, 104019, 104051,
runs = [110934,108666, 105130, 104131, 103322, 102893]
        
"""
Step 2:
Do you want to look at the CIS scans for the last month?  If you set 'scans' equal to true, then you will have plots of the CIS scans for all channels that failed to be calibrated. However, be mindful that this will load in CIS scans for each channel for each run.  This can be slow (~30 minutes).
"""

useScans = True  #  Read in the CIS scan graphs in order to plot them?

#
#  Experts only below.
#

# Define a plot of qflag and DB problems shown over whole detector
p = PlotModuleChannel(runType2='CIS', parameter2='isCalibrated_qflag',   text2='1% DB Variation', invert2=False,\
                      runType1='CIS', parameter1='isCalibrated', text1='DB Variation and QFlag Failure', invert1=True)

x = ReadCIS(getScans=True,)

# This is the worker processing loop.
Go([\
    Use(run=runs, runType='CIS'),             # Tell Tucs which runs to use                       \
    x,           # Tell Tucs to load in the CIS information          \
    ReadDB(),                                 # Read CIS constants from DB as well                \
    CalibBadBit(),                            # Set the 'isCalibrated' flag for each channel      \
#    ScanProbability(),                        \
    DumpCISUncalibrated(),                    # Write out CIS scans for uncalibrated channels     \
    p,                                        # Make a plot showing problems: module v.s. channel \
    TimeStability(),                          # Make time stability plots
    IsEntireModuleBad(),                      # If there is an entire module bad, print warning   \
    ])


