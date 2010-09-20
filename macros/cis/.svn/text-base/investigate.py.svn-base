# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This macro is used for investigating problems with CIS.  There are
# many ways to configure it, which are documented below.  Plots will
# be created that summarize channel and detector status
#
# March 04, 2009
#

execfile('src/load.py', globals()) # don't remove this!


"""
Step 1:
You need to set the 'runs' variable to reflect which runs you would like to look at.  There are various examples below: the set of runs used for the Feb. 2009 TileWeek, the set of runs of Feb. 27th 2009, and the last example uses the last month of CIS runs.
"""

#runs = [100489,100699,100814,101152,101441,102009] # Runs as of Feb. 2009 TileWeek
#runs = '-2 months'                                 # Use the last month of runs
#runs = 101184                                      # Just use one run, useful for debugging
runs = '-28 days'                                  # use the last 28 days of runs
        
"""
Step 2:
Do you want to look at the CIS scans for the last month?  If you set 'scans' equal to true, then you will have plots of selected CIS scans. However, be mindful that this will load in CIS scans for each channel for each run.  This can be slow if you selected the whole detector and many runs.
"""

useScans = True  #  Read in the CIS scan graphs in order to plot them?

"""
Step 3:
Use samples?  This is insanely slow, but set the following variable to 'True' if you want the samples.
UChicago people: This doesn't work on pcucatlas08 (try pcucatlas05)
"""

useSamples = False

"""
Step 4:
Show scans for all selected channels?  If set to 'False', then only scans for problematic reasons will be loaded.
"""

showAll = True

"""
Step 5:
Which regions in the detector do you want to look at?  If you select nothing, the whole detector is used.
"""

#selected_region = ''                     # Look at who detector
#selected_region = 'EBA_m10'              # Look at just EBA
#selected_region = 'LBA_m05_c00_lowgain' # Look at LBA module 42 channel 0 lowgain
#selected_region = 'lowgain'             # Look at low-gain only
#selected_region = 'EBC_m43_p18_lowgain' # One can only use 'PMT' numbering, like PMT18 here

"""
step 6
dump debug information.  This will print all information in TUCS to the screen (stdout) and will save the detector tree, if you want to reload all information in Tucs with the LoadTree() worker
"""
debug = False

#
#  Experts only below.
#

if not globals().has_key('selected_region'):
    selected_region = ''

if useSamples:
    gs = GetSamples(all=showAll)
else:
    gs = None

if useScans:
    gs2 = GetScans(all=showAll)
    scs = ShowCISScans(all=showAll)
else:
    gs2 = None
    scs = None

if debug:
    pr = Print()
    s = SaveTree()
else:
    pr = None
    s = None

p = PlotModuleChannel(runType1='CIS', parameter1='goodRegion',         text1='Low-gain',         invert1=True,\
                      runType2='CIS', parameter2='goodRegion', text2='High-gain', invert2=True)

Go([\
    Use(run=runs, runType='CIS', region=selected_region),\
    ReadCIS(),
    ReadDB(),\
    CISFlagProcedure(),\
    CISRecalibrateProcedure(),\
    p,\
    gs,\
    gs2,\
    scs,\
    ShowResiduals(),\
    IsEntireModuleBad(),\
    IsEntirePartitionBad(),\
    pr,\
    s,\
    ])
