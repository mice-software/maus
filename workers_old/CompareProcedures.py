# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# June 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import os, datetime

class CompareProcedures(GenericWorker):
    "Compare flag procedure and calibration procedure"

    def __init__(self, runType):
        self.runType = runType

    def ProcessStart(self):
        self.nPP = 0
        self.nPF = 0
        self.nFP = 0
        self.nFF = 0
        self.n = 0
        
    def ProcessStop(self):
        if self.n:
            print 'Total number of calibrations:              ', self.n
            print 'Pass Flag Pass Calib', self.nPP, 'or', float(self.nPP)/self.n*100, '%'
            print 'Pass Flag Fail Calib', self.nPF, 'or', float(self.nPF)/self.n*100, '%'
            print 'Fail Flag Pass Calib', self.nFP, 'or', float(self.nFP)/self.n*100, '%'
            print 'Fail Flag Fail Calib', self.nFF, 'or', float(self.nFF)/self.n*100, '%'
    
    def ProcessRegion(self, region):
        chanstat_event = None
        for event in region.GetEvents():
            if event.runType == "ChanStat":
                chanstat_event = event

        for event in region.GetEvents():
            if event.runType == self.runType:
                assert(event.data.has_key('goodRegion'))
                assert(event.data.has_key('calibratableRegion'))

                self.n += 1

                if event.data['goodRegion'] and event.data['calibratableRegion']:
                    self.nPP += 1
                elif  event.data['goodRegion'] and not event.data['calibratableRegion']:
                    self.nPF += 1
                    #if chanstat_event != None:
                    #    print region.GetHash(), chanstat_event.problems
                    #else:
                    #    print region.GetHash(), "no probs"
                    #print 'Pass ChiRms Fail Qflag', region.GetHash(), region.GetHash(1)
                elif not event.data['goodRegion'] and event.data['calibratableRegion']:
                    self.nFP += 1
                    #print 'Fail ChiRms Pass Qflag', region.GetHash(), region.GetHash(1)
                elif not event.data['goodRegion'] and not event.data['calibratableRegion']:
                    self.nFF += 1

                return
                                
        
        
