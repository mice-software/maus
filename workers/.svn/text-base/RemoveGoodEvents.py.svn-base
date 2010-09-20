# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *

class RemoveGoodEvents(GenericWorker):
    'Only keep bad channels'

    def __init__(self, runType = 'CIS', parameter='isBad', invert=False):
        self.runType = runType
        self.parameter = parameter
        self.invert = invert

    def ProcessRegion(self, region):
        newevents = set()
        for event in region.events:
            if event.data.has_key(self.parameter):
                if event.data[self.parameter] and not self.invert:
                    newevents.add(event)
                elif not event.data[self.parameter] and self.invert:
                    newevents.add(event)
                                            

        region.events = newevents
