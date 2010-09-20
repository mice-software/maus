# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# Dump all of the bad CIS scans for some run
#
# March 05, 2009
#
execfile('src/load.py', globals()) # don't remove this!

u = Use(run=90555)
p = Print()

processors = [u, ReadChanStat(), RemoveGoodEvents(), p,\
              u, ReadCISFile(getScans=True, cut=3),\
              ShowCISScans()]

Go(processors)
