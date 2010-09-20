# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *

class Print(GenericWorker):
    "A class for printing calibration output"

    printRunType   = True
    printRunNumber = True
    printTime      = True
    printData      = True
    printRegion    = True

    def __init__(self,verbose=False,type='readout'):
        self.verbose = verbose
        self.type = type
        

    def printOptions(self, printRunType = True, printRunNumber=True,
                     printTime = True, printData = True, printRegion = True):
        self.printRunType = printRunType
        self.printRunNumber = printRunNumber
        self.printTime = printTime
        self.printData = printData
        self.printRegion = printRegion

    def SetVerbose(self, verbose=True):
        self.verbose = verbose

    def ProcessRegion(self, region):
        if len(region.GetEvents()) == 0:
            return

        print 'Region:', region.GetHash()
        for event in region.GetEvents():
            if self.verbose:
                print '\tEvent:'                
                if self.printRunType:   print '\t\tType:', event.runType,
                if self.printRunNumber: print '\t\tRun:', event.runNumber,
                if self.printTime:      print '\t\tTime:', event.time,
                if self.printData:
                    print '\t\tData: {',
                    for key, value in event.data.iteritems():
                        print '\t',key,':\t', value
                    print '}',
                    print
            else:
                if self.printRunType:   print event.runType, ',',
                if self.printRunNumber: print event.runNumber, ',',
                if self.printTime:      print event.time, ',',
                if self.printData:
                    for key, value in event.data.iteritems():
                        print key,':', value,',',
                    print
                

        
