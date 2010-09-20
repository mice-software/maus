# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *

class PrintDB(GenericWorker):
    "A class for printing calibration DB values"

    def ProcessRegion(self, region):
        if len(region.GetEvents()) == 0:
            return

        print region.GetHash(), region.GetHash(1)
        for event in region.GetEvents():
            if event.data.has_key('calibration_db'):
                print '\t%s: %f' % (event.runType, event.data['calibration_db'])
                

        
