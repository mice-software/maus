
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

#runs = [100489,100699,100814,101152,101441,102009] # Runs as of Feb. 2009 TileWeek
#runs = [101184, 101441, 101784, 101785, 102009] # Runs as of Feb. 27, 2009
runs = '-1 month' # Use the last month of runs
#runs = [101184]  # Just use one run, useful for debugging
#runs = [102208, 102501, 102893]
#runs = [104131]


u = Use(run=runs, useDateProg=True, runType='CIS', verbose=False)
x = ReadCISFile()
r = ReadDB()
c = CalibBadBit()

# whole calibration scheme
parameter = 'isCalibrated'
text = 'Fail CIS Procedure'
p = PlotModuleChannel(runType2='CIS', parameter2=parameter,   text2=text, invert2=True,\
                      runType1='CIS', parameter1='isDefaultCalib', text1='Default Calib.', invert1=False)

#Go[u, x, r, c, IsDefaultCalib(), p])

p = PlotModuleChannel(runType2='CIS', parameter2=parameter,   text2=text, invert2=True,\
                      runType1='Repair', parameter1='isBad', text1='Replaced 3in1', invert1=False)

#Go[u, x, r, c, ReadRepair3in1List(), p])

p = PlotModuleChannel(runType2='CIS', parameter2=parameter,   text2=text, invert2=True,\
                      runType1='Repair', parameter1='isBad', text1='Intervened Module', invert1=False)

#Go[u, x, r, c, ReadRepairModuleList(), p])


# just qflag problems
parameter = 'isCalibrated_qflag'
invert = False
text = '1% DB Variation of Good Calib.'

remove_default = RemoveGoodEvents(parameter='isDefaultCalib', invert=True)

p = PlotModuleChannel(runType2='CIS', parameter2=parameter,   text2=text, invert2=invert,\
                      runType1='Repair', parameter1='isBad', text1='Intervened Module', invert1=False)

Go([u, x, r, c, ReadRepairModuleList(), IsDefaultCalib(), remove_default, p])


remove_default = RemoveGoodEvents(parameter='isDefaultCalib', invert=True)

p = PlotModuleChannel(runType2='CIS', parameter2=parameter,   text2=text, invert2=invert,\
                      runType1='CIS', parameter1='isDefaultCalib', text1='Default Calib.', invert1=False)

#Go[u, x, r, c, IsDefaultCalib(), p])

# rms
p1 = PlotModuleChannel(runType2='CIS', parameter2=parameter,   text2=text, invert2=invert,\
                      runType1='CIS', parameter1='isNoisy', text1='Large injection RMS', invert1=False)

# chi2 lo
p2 = PlotModuleChannel(runType2='CIS', parameter2=parameter,   text2=text, invert2=invert,\
                      runType1='Repair', parameter1='isChi2Low', text1='Low Probability Chi2', invert1=False)

#chi2 hi
p3 = PlotModuleChannel(runType2='CIS', parameter2=parameter,   text2=text, invert2=invert,\
                      runType1='Repair', parameter1='isChi2Hi', text1='High Probability Chi2', invert1=False)

#Go[u, x, r, c, ScanProbability(), p1, p2, p3])

p = PlotModuleChannel(runType2='CIS', parameter2=parameter,   text2=text, invert2=invert,\
                      runType1='None', parameter1='None', text1='None', invert1=False,)

#Go[u, x, r, c, IsDefaultCalib(), remove_default, p])

p = PlotChannelsInModules(runType='CIS', parameter=parameter,   text=text, invert=invert,)
#Go[u, x, r, c, IsDefaultCalib(), p])
