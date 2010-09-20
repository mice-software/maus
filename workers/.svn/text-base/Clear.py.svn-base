# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# January 04, 2009
#

from src.GenericWorker import *

class Clear(GenericWorker):
    "Clear out all events to start fresh"

    def ProcessRegion(self, region):
        region.SetEvents(set())
