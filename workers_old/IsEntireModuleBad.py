# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
import src.MakeCanvas

class IsEntireModuleBad(GenericWorker):
    "Checks to see if isBad is true for an entire module"

    def __init__(self, runType = 'CIS'):
        self.runType = runType

    def ProcessStart(self):
        self.nBad = 0

    def ProcessStop(self):
        print 'There are', self.nBad, 'modules with only bad events'

    def ProcessRegion(self, region):
        if 'c' in region.GetHash() or 'm' not in region.GetHash(): 
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
            self.nBad += 1
                    
                    

