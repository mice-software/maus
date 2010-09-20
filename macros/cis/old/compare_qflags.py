# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Compare the new and old quality CIS quality flags
# 
# March 04, 2009
#
execfile('src/load.py', globals()) # don't remove this!

sregion = ''  # Only look at a certain region?  Otherwise, ''
runs = [101184, 101441, 101784, 101785, 102009]

u1 = Use(run=runs, runType='CIS', region=sregion)

x1 = ReadCISFile(cut=3)

Go([u1, x1, CompareQFlags()])

