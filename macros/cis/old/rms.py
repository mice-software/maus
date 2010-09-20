# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Not sure what this does anymore.  I think it makes a distribution of the fixed-charge RMS and the fit Chi2.
#
# March 04, 2009
#

execfile('src/load.py', globals()) # don't remove this!


"""
Step 1:
You need to set the 'runs' variable to reflect which runs you would like to look at.  There are various examples below: the set of runs used for the Feb. 2009 TileWeek, the set of runs of Feb. 27th 2009, and the last example uses the last month of CIS runs.
"""

#runs = [100489,100699,100814,101152,101441,102009] # Runs as of Feb. 2009 TileWeek
runs = '-1 month' # Use the last month of runs
#runs = [101184]  # Just use one run, useful for debugging
#runs = [104131]
#runs = [103347, 104675, 104835, 104934, 102208]
#runs = [104467]
runs = '-2 week'
runs = [110934]
        
"""
Step 2:
Do you want to look at the CIS scans for the last month?  If you set 'scans' equal to true, then you will have plots of the CIS scans for all channels that failed to be calibrated. However, be mindful that this will load in CIS scans for each channel for each run.  This can be slow (~30 minutes).
"""

useScans = True  #  Read in the CIS scan graphs in order to plot them?

"""
Step 3:
Which regions in the detector do you want to look at?
"""

selected_region = '' # Look at who detector
#selected_region = 'LBA' # Look at just LBA
#selected_region = 'LBA_m42_c0_lowgain' # Look at LBA module 42 channel 0 lowgain
#selected_region = 'lowgain' # Look at low-gain only
#selected_region = 'EBC_m06_c30_lowgain'

#
#  Experts only below.
#

# Define a plot of qflag and DB problems shown over whole detector
p = PlotModuleChannel(runType2='CIS', parameter2='isCalibrated_qflag',   text2='1% DB Variation', invert2=False,\
                      runType1='CIS', parameter1='isCalibrated', text1='DB Variation and QFlag Failure', invert1=True)

x = ReadCIS(getScans=useScans, getScansRMS=True)

#for partition in ['EBA', 'LBC', 'EBC', 'LBC']:
# This is the worker processing loop.
Go([\
    Use(run=runs, runType='CIS', region=selected_region),       # Tell Tucs which runs to use                       \
    x,                                                          # Tell Tucs to load in the CIS information          \
    ReadDB(),
    ScanProbability(),
    CompareCISCuts()
    ])
"""
    CISFlagProcedure(),
    TimeStability(all=True),
    CISRecalibrateProcedure(),
    ShowCISScans(all=True),
#    Print(),
    IsEntireModuleBad(),
    IsEntirePartitionBad(),
    GetInjections(),
    SaveTree(filename='/tmp/ctunnell/rms.p')])

    CalibBadBit(),                            # Set the 'isCalibrated' flag for each channel      \
    ScanProbability(),                        \
    DumpCISUncalibrated(),                    # Write out CIS scans for uncalibrated channels     \
    p,                                        # Make a plot showing problems: module v.s. channel \
    TimeStability(),                          # Make time stability plots
    IsEntireModuleBad(),                      # If there is an entire module bad, print warning   \
    SaveTree(filename='investigate.p'),
    ])


"""
