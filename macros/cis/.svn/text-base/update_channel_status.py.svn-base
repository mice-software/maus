# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This is a script intended to provide summary status information about the
# CIS system in the form of a channel status bit.  This does NOT update the
# database directly.  Instead, it creates a local SQLite file.
#
# March 04, 2009
#

execfile('src/load.py', globals()) # don't remove this!


"""
Step 1:
You need to set the 'runs' variable to reflect which runs you would like to look at.  There are various examples below: the set of runs used for the Feb. 2009 TileWeek, the set of runs of Feb. 27th 2009, and the last example uses the last month of CIS runs.
"""
#runs = [72652, 73305, 72653, 72661, 79259, 78023, 79781, 78026]
runs = '-28 day'
#runs= [104467,104675,104835,104934,105130,107141,108481,108566,108666,110934,111291,112497,112922,113131,113236,113248,113284,113469] #Wed Apr 29 16:28:49 CEST 2009

#runs =[102893,103322, 103347,103864,104019,104051,104131,104274,104467,104675,104835,104934,105130,107141,108481,108566,108666] # Wed Apr 15 16:28:52 CEST 2009

#runs=[102208, 102217,102501,102893,103322,103347,103864,104019,104051,104131,104274,104467,104675,] # april 1 09  

#runs=108666
#
"""
Step 2:
Do you want to look at the CIS scans for the last month?  If you set 'scans' equal to true, then you will have plots of the CIS scans for all channels that failed to be calibrated. However, be mindful that this will load in CIS scans for each channel for each run.  This can be slow (~30 minutes).
"""
showAll=False
useScans = False  #  Read in the CIS scan graphs in order to plot them?

#
#  Experts only below.
#

if useScans:
    gs2 = GetScans(all=showAll)
    scs = ShowCISScans(all=showAll)
else:
    gs2 = None
    scs = None

# Use the whole detector if no selected region is specified
if not globals().has_key('selected_region'):
    selected_region = ''
    
print 'Dumping plots for %s' % selected_region
Go([Use(run=runs, runType='CIS', region=selected_region),\
    ReadCIS(),\
    ReadDB(),\
    CISFlagProcedure(),\
    CISRecalibrateProcedure(),\
    gs2,\
    scs,\
    CopyLocalChanStat(),\
    WriteChanStat()
    ])


