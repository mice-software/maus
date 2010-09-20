# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Create an SQLite file with new CIS constants
#
# March 05, 2009
#

execfile('src/load.py', globals()) # don't remove this!
import gc

runs = '-4 weeks' # Use the last month of runs
runs = [115207,115267,115408,115470,115500,115958,116286,116601,116742,117591,118255]
#runs = [117591, 116742, 116601, 116286]
#runs = [102208, 102501, 102893, 103322, 103347, 103864, 104019, 104051, 104131]
#runs = [103347]

recalibrate_all = True

u = Use(run=runs, useDateProg=True, runType='CIS', verbose=False,)
x = ReadCIS()
r = ReadDB()

details = False
debug = True
# declaring it as 'g' helps memory collection
#selected_region='LBA_m25'

if not globals().has_key('selected_region'):
    selected_region = ''

if details:
    p = PlotModuleChannel(runType1='CIS', parameter1='calibratableRegion',         text1='Low-gain',         invert1=True,\
                          runType2='CIS', parameter2='calibratableRegion', text2='High-gain', invert2=True)
    c =  CompareProcedures('CIS')
    g = GetScans()
    s = ShowCISSCans()
else:
    p = c = g = s = None

if debug:
    dp = Print()
    ds = SaveTree()
else:
    dp = ds = None
    
Go([u, SelectRegion(selected_region), x, r, CISRecalibrateProcedure(all=recalibrate_all), CISFlagProcedure(dbCheck=False), p, IsEntireModuleBad(), IsEntirePartitionBad(), p, c, g, s, CopyDBConstants('CIS', recreate=True), WriteDB(), dp, ds])
        


