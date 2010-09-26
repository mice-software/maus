# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
import src.MakeCanvas

class IsEntirePartitionBad(GenericWorker):
    "Checks to see if isBad is true for an entire partition"

    def __init__(self, runType = 'CIS'):
        self.runType = runType

    def ProcessRegion(self, region):
        if ('EBA' not in region.GetHash() and\
            'LBA' not in region.GetHash() and\
            'EBC' not in region.GetHash() and\
            'LBC' not in region.GetHash()) or\
            '_m' in region.GetHash():
            return

        found = False
        good = False
        for event in region.RecursiveGetEvents():
            if event.runType == self.runType and event.data.has_key('goodRegion'):
                found = True
                if event.data['goodRegion']:
                    good = True

        if found and not good:
            print 'No good events within: ', region.GetHash()
                    
                    

