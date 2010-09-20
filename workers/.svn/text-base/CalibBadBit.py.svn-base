# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from math import *
import src.MakeCanvas

class CalibBadBit(GenericWorker):
    "Set the CalibBat bit for some runType"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self, runType = 'CIS', deviation = True):
        self.runType = runType
        self.deviation = deviation

    def ProcessStart(self):
        self.ncalib = 0
        self.nuncalib_qflag = 0
        self.nuncalib_db = 0
        self.nuncalib_both = 0

        self.nPP = 0
        self.nPF = 0
        self.nFP = 0
        self.nFF = 0

    def ProcessStop(self):
        print 'Calibrated:', self.ncalib
        print 'Uncalibrated qflag:', self.nuncalib_qflag
        print 'Uncalibrated DB:', self.nuncalib_db
        print 'Uncalibrated both:', self.nuncalib_both

        print 'passO passN', self.nPP
        print 'passO failN', self.nPF
        print 'failO passN', self.nFP
        print 'failO failO', self.nFF
        
    
    def ProcessRegion(self, region):
        if 'gain' not in region.GetHash():
            return

        found = False
        foundpass = False
        foundpass_qflag = False
        foundpass_db = False

        goodRegion = False

        for event in region.GetEvents():
            if event.runType == self.runType:
                variable = 'calibratableRegion'
                if event.data.has_key(variable):
                    goodRegion = event.data[variable]

                    
                if event.data.has_key('quality_flag'):
                    found = True
                    
                    if event.data['quality_flag'] & 27 == 27 and\
                           (not self.deviation or abs(event.data['deviation']) < 0.01):
                        foundpass = True
                        break
                    elif event.data['quality_flag'] & 27 == 27:
                        foundpass_qflag = True
                    elif not self.deviation or abs(event.data['deviation']) < 0.01:
                        foundpass_db = True

        if found:
            newevents = set()
              
            if foundpass:
                self.ncalib += 1
            elif foundpass_qflag and not foundpass_db:
                self.nuncalib_db += 1
            elif not foundpass_qflag and foundpass_db:
                self.nuncalib_qflag += 1
            else:
                self.nuncalib_both += 1

            if foundpass and goodRegion:
                self.nPP += 1
            if not foundpass and goodRegion:
                self.nFP += 1
            if foundpass and not goodRegion:
                self.nPF += 1
                for events in region.GetEvents():
                    event.data['moreInfo'] = True
                    newevents.add(event)
                region.SetEvents(newevents)
                
            if not foundpass and not goodRegion:
                self.nFF += 1

