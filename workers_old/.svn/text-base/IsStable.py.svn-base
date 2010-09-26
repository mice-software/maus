# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from math import *
import src.MakeCanvas

class IsStable(GenericWorker):
    "Determine if channel is stable"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self, runType = 'CIS', threshold = 0.003):
        self.runType = runType
        self.threshold = threshold # stability requirement, recalib bit threshold

        # stable counters
        self.nstable = 0
        self.nnostable = 0

        # stable counters with default calib
        self.nstableDefault = 0
        self.nnostableDefault = 0

        # stable counters with DB variation
        self.nstableDB = 0
        self.nnostableDB = 0

        # stable counters with DB variation
        self.nstableDB_repair = 0
        self.nnostableDB_repair = 0
        
    def ProcessStop(self):
        print 'Stable:    ', self.nstable
        print 'Not stable:', self.nnostable

        if self.nstableDefault or self.nnostableDefault:
            print 'Out of these:'
            print '\tStable (with default calib):    ', self.nstableDefault
            print '\tNot stable (with default calib):', self.nnostableDefault

        if self.nstableDB or self.nnostableDB:
            print 'Out of these:'
            print '\tStable (with DB variation):    ', self.nstableDB
            if self.nstableDB_repair or self.nnostableDB_repair:
                print '\t\tRepairs:', self.nstableDB_repair
            print '\tNot stable (with DB variation):', self.nnostableDB
            if self.nstableDB_repair or self.nnostableDB_repair:
                print '\t\tRepairs:', self.nnostableDB_repair
                                
            

            
                        
    
    def ProcessRegion(self, region):
        if 'gain' not in region.GetHash():
            return

        found = False
        stable = False

        default = False
        db = False
        repair = False

        for event in region.events:
            if event.runType == 'Repair':
                if event.data['isBad']:
                    repair = True
                    
            if event.runType == self.runType:
                if event.data.has_key('rms') and event.data.has_key('mean'):
                    found = True
                    if event.data['rms']/event.data['mean'] < self.threshold:
                        stable = True
                    else:
                        stable = False

                    if event.data.has_key('isDefaultCalib'):
                        if event.data['isDefaultCalib']:
                            default = True

                    if event.data.has_key('isCalibrated_qflag'):
                        if event.data['isCalibrated_qflag']:
                            db = True
                            
                    break
                        
        if found:
            if stable:
                self.nstable += 1
                if default:
                    self.nstableDefault += 1

                if db:
                    self.nstableDB += 1

                    if repair:
                        self.nstableDB_repair += 1
            else:
                self.nnostable += 1
                if default:
                    self.nnostableDefault += 1
                if db:
                    self.nnostableDB += 1

                    if repair:
                        self.nnostableDB_repair += 1
                            
            newevents = set()
            for event in region.events:
                if event.runType == self.runType:
                    event.data['IsStable'] = stable
                newevents.add(event)
                                                                        
            region.events = newevents
