# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This creates the CIS public plot that shows the channel deviation after
# many months.  
#
# March 04, 2009
#

execfile('src/load.py', globals()) # don't remove this!


"""
Step 1:
You need to set the 'runs' variable to reflect which runs you would like to look at.  There are various examples below: the set of runs used for the Feb. 2009 TileWeek, the set of runs of Feb. 27th 2009, and the last example uses the last month of CIS runs.

You have to specify two sets of runs, and this script compares them.  The sets of runs are called 'runs' and 'runs2'
"""

#runs = [100489,100699,100814,101152,101441,102009] # Runs as of Feb. 2009 TileWeek
#runs = '-28 day' # Use the last month of runs
#runs = [113469,113728,114171,114371,114618,114683,114723,114940,114941,115207,115267,115408,115470,115500,115958,116286,116600,116601] # last month from may 27
runs = [90555,90555,90860,91315,91567,91837,92016,92416,92971,92972] # novem 08 
runs2 =[79781,79786,79789,79797,80487,80493,80494,80495,80512,80517,80529,80534,81342,81349,81355,81363,81601,81608,81627,81632,82395,82401,82405,82414,83033,83051,83066,83078,84402,84408,84432,84450,84826] # aug 2008

"""
Step 2:
Which regions in the detector do you want to look at?  If you select nothing, the whole detector is used.
"""

#selected_region = '' # Look at who detector
#selected_region = 'EBA' # Look at just EBA
#selected_region = 'LBA_m42_c0_lowgain' # Look at LBA module 42 channel 0 lowgain
#selected_region = 'lowgain' # Look at low-gain only
#selected_region = 'EBC_m33_c08_lowgain'
#selected_region = 'LBA_m30_c32_lowgain'
#selected_region = 'EBA'   

"""
step 3
dump debug information
"""
debug = False

#
#  Experts only below.
#

if not globals().has_key('selected_region'):
    selected_region = ''

if debug:
    pr = Print()
    s = SaveTree()
else:
    pr = None
    s = None

h = HistoryPlot('CIS')

Go([\
    Use(run=runs, runType='CIS', region=selected_region),\
    ReadCIS(),\
    CISFlagProcedure(dbCheck=False),\
    pr,
    h,
    Clear(),
    Use(run=runs2, runType='CIS', region=selected_region),\
    ReadCIS(),\
    CISFlagProcedure(dbCheck=False),\
    h,\
    pr,\
    s,\
    ])
