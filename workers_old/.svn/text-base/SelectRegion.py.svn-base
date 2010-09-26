# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *

class SelectRegion(GenericWorker):
    "A class for selecting events from only certain region"

    def __init__(self, selectedRegion):
        self.selectedRegion = selectedRegion
        
    def ProcessRegion(self, region):
        if self.selectedRegion not in region.GetHash():
            region.SetEvents(set())
        
