# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# The macro dumps the calibration constants for some run
#

execfile('src/load.py', globals()) # don't remove this!

"""
Step 1:
Choose a run or some runs to analyze (only CIS at present)
"""
runs = 102893             # CIS run
#runs = [102893,103322,]  # two CIS runs
#runs = "-1 week"         # last week of CIS runs

"""""
Step 2:
Which regions in the detector do you want to look at?
"""

#selected_region = ''                   # Look at who detector
#selected_region = 'EBA'                # Look at just EBA
#selected_region = 'LBA_m42_c0_lowgain' # Look at LBA module 42 channel 0 lowgain
#selected_region = 'lowgain'            # Look at low-gain only
selected_region = 'LBC_m20_c10'


##
## Experts only below
##

# Use the whole detector if no selected region is specified
if not globals().has_key('selected_region'):
    selected_region = ''
        
Go([       
    Use(run=runs, runType='CIS', region=selected_region),
    ReadCIS(),
    ReadDB(),
    PrintDB(),
    ])     


