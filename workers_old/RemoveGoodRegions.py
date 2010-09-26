# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *

class RemoveGoodRegions(GenericWorker):
    'Only keep bad channels'

    def __init__(self, runType = 'CIS', parameter='isBad', invert=False):
        self.runType = runType
        self.parameter = parameter
        self.invert = invert

    def ProcessRegion(self, region):
        good = False
        
        for event in region.events:
            if event.data.has_key(self.parameter):
                print even.data[self.parameter]
                if event.data[self.parameter] and not invert:
                    good = True
                elif not event.data[self.parameter] and invert:
                    good = False
                                            

        if good:
            region.events = set()
