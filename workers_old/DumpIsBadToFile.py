# Author: Michael Miller <miller@uchicago.edu>
#
# A class for dumping a text file of bad channels in calibration
# runs
#
# February 27, 2009
#

from src.GenericWorker import *

class DumpIsBadToFile(GenericWorker):
    "A class for dumping a text file of bad channels in calibration runs"

    def __init__(self, runType):
        self.runType = runType
        
    def ProcessStart(self):
        self.fileName = self.runType + "_isBad.txt" 
        self.fout = open( self.fileName, "w")

    def ProcessStop(self):
        self.fout.close()


    def ProcessRegion(self, region):
        if len(region.GetEvents()) == 0:
            return

        for event in region.GetEvents():
            if event.runType != self.runType:
                continue

            if (event.data.has_key('isBad') and event.data['isBad']) or\
                   (event.data.has_key('goodRegion') and not event.data['goodRegion']):
                x, y, z, w = region.GetNumber(1)
                self.fout.write('%d %02d %02d\n' % (x-1, y, z))
                return

                
        
