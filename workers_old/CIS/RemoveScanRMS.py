# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import os, datetime

class RemoveScanRMS(GenericWorker):
    "remove scan rms"

    def ProcessRegion(self, region):
        if region.events == set():
            return
        newevents = set()
        for event in region.events:
            if event.runType == 'CIS' and event.data.has_key('dumpScan'):
                if event.data['scan_rms']:
                    del event.data['scan_rms']
            newevents.add(event)
        region.events = newevents
                    
