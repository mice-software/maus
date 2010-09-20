# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

import time

class Event:
    "This is an event"

    def __init__(self, runType, runNumber, data, time):
        self.runType   =  runType
        self.runNumber =  runNumber
        self.data      =  data
        self.time      =  time

    def getTimeSeconds(self):
        if not self.time:
            print 'wtf?'
            return 0.0

        c = time.strptime(self.time, '%Y-%m-%d %H:%M:%S')
        return time.mktime(c)
    
