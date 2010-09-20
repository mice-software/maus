# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *

# this file needs a new home.  it's part of a hack to save time before a tlak!

class RemoveSpecial(GenericWorker):
    'Only keep bad channels'

    def ProcessRegion(self, region):
        keep = False
        
        for event in region.events:
            if event.runType == 'CIS' and event.data.has_key('isCalibrated'):
                if not event.data['isCalibrated']:
                    keep = True

            if event.runType == 'DQ' and event.data.has_key('isBad'):
                if event.data['isBad']:
                    keep = True
                
        if not keep:
            region.events = set()
