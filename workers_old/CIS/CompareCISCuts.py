# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import os, datetime

class CompareCISCuts(GenericWorker):
    "Compare CIS cuts of Chi2/RMS versus Likely Calib/Max-response"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self):
        pass


    def ProcessStart(self):
        self.nPP = 0
        self.nPF = 0
        self.nFP = 0
        self.nFF = 0

        self.n = 0
        
    def ProcessStop(self):
        if self.n:
            print 'Total number of calibrations:              ', self.n
            print 'Pass ChiRMS Pass QFlag', self.nPP, 'or', float(self.nPP)/self.n*100, '%'
            print 'Pass ChiRMS Fail QFlag', self.nPF, 'or', float(self.nPF)/self.n*100, '%'
            print 'Fail ChiRMS Pass QFlag', self.nFP, 'or', float(self.nFP)/self.n*100, '%'
            print 'Fail ChiRMS Fail QFlag', self.nFF, 'or', float(self.nFF)/self.n*100, '%'
    
    def ProcessRegion(self, region):
        # Only look at each gain within some channel
        if 'gain' not in region.GetHash():
            return

        newevents = set()
        for event in region.GetEvents():
            pass_chirms = True
            pass_qflag  = True

            if event.runType == 'CIS':
                assert(event.data.has_key('problems'))
                assert(isinstance(event.data['problems'], dict))

                problems = event.data['problems']
                
                if problems['Digital Errors']:
                    continue
                
                if problems['Large Injection RMS']:
                    pass_chirms = False
                
                if problems['Low Chi2']:
                    pass_chirms = False
                
                if problems['Fail Max. Point']:
                    pass_qflag = False
                
                if problems['Fail Likely Calib.']:
                    pass_qflag = False
                
                self.n += 1

                if pass_chirms and pass_qflag:
                    self.nPP += 1
                elif pass_chirms and not pass_qflag:
                    self.nPF += 1
                    #print 'Pass ChiRms Fail Qflag', region.GetHash(), region.GetHash(1)
                elif not pass_chirms and pass_qflag:
                    self.nFP += 1
                    #print 'Fail ChiRms Pass Qflag', region.GetHash(), region.GetHash(1)
                elif not pass_chirms and not pass_qflag:
                    self.nFF += 1
                                
                event.data['comp_chi_rms'] = [pass_chirms, pass_qflag]

            newevents.add(event)
        region.SetEvents(newevents)
        
        
