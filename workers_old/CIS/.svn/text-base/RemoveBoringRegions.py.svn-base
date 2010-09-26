# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *

class RemoveBoringRegions(GenericWorker):
    'Remove regions that are not of interest for the channel status flag'

    def ProcessRegion(self, region):
        for event in region.GetEvents():
            if event.runType == 'CIS' and\
               event.data.has_key('goodRegion') and\
               event.data['goodRegion']:
                region.SetEvents(set())
                return

