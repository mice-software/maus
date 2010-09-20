# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# This macro makes the CIS public plot that shows the distribution of
# calibration constants.  There are two 1D histograms (one per gain) and
# each entry is a channel's calibration constant.
#
# June 20, 2009
#

execfile('src/load.py', globals()) # don't remove this!

runs = '-4 week' # Use the last month of runs
#runs = [90555,90555,90860,91315,91567,91837,92016,92416,92971,92972] # novem 08

Go([
    Use(run=runs, runType='CIS'),      # Tell TUCS to use CIS runs, and which runs
    ReadCIS(),                         # Read in CIS constants and quality flags
    ReadDB(),                          # Read in constants from COOL DB
    CISRecalibrateProcedure(all=True), # Run calibration procedure
    CalibDist(),                       # Generate calibration distribution plot 
    ])

