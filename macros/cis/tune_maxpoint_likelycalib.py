
# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Tune the maximum value in fit range quality flag and also the likely calib flag
#
# March 05, 2009
#

execfile('src/load.py', globals()) # don't remove this!

runs = '-1 month' # Use the last month of runs
#runs = [102208, 102501, 102893, 103322, 103347, 103864, 104019, 104051, 104131]
runs = [87920, 88366, 88843, 89767, 90170, 90179, 90364, 90555, 90860, 91315]

u = Use(run=runs, runType='CIS')
x = ReadCIS()
r = GetScans(all=True)
t = TuneCutsMaxPointLikelyCalib()

Go([u,x,ReadDB(),CISFlagProcedure(),r,t,])

