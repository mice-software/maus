# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Tune the fixed-charge RMS and chi2 quality flags
#
# March 05, 2009
#

execfile('src/load.py', globals()) # don't remove this!

runs = '-1 month' # Use the last month of runs
#runs = [102208, 102501, 102893, 103322, 103347, 103864, 104019, 104051, 104131]
runs = [103347]

u = Use(run=runs, runType='CIS')
x = ReadCIS()
r = GetScans(all=True,getScansRMS=True)
t = TuneCuts()

Go([u,x,ReadDB(),CISFlagProcedure(),r,t,])

