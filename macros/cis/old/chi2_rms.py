
# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Make a TH2F with module versus channel number for each partition.
# Points will be placed for CIS problems and DQ problems, to correlate.
#
# Note: This doesn't work at the moment since PlotModuleChannel() changed.
#
# March 05, 2009
#

execfile('src/load.py', globals()) # don't remove this!

#runs = '-1 month' # Use the last month of runs
runs = [102208, 102501, 102893, 103322, 103347, 103864, 104019, 104051, 104131]
runs = [103347]
runs = [104131]



u = Use(run=runs, useDateProg=True, runType='CIS', verbose=False, )
x = ReadNewCISFile(processingDir='/tmp/ctunnell/recis', getScans=True, getScansRMS=True)
r = ReadDB()
c = CalibBadBit()

Go([u, x, r, c, ScanProbability(), PlotModuleChannelValue(runType='CIS', parameter='numHiRMS', text='Injection RMS > 5 counts')])


