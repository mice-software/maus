# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *
from math import *

class ReadCIS(ReadGenericCalibration):
    "Read in CIS calibration factors and quality flags from CIS calibration ntuples"

    def __init__(self, processingDir='/afs/cern.ch/user/t/tilecali/w0/ntuples/cis', turnOffStatCuts=False):
        self.processingDir = processingDir
        self.ftDict = {} # Used for linear constant.  Each element is a [TTree, TFile]
        self.badRuns = set()
        self.turnOffStatCuts = turnOffStatCuts

    def get_index(self, ros, mod, chan, gain):
        return ros  *64*48*2\
            + mod      *48*2\
            + chan        *2\
            + gain

    def ProcessStart(self):
        pass

    def ProcessStop(self):
        pass
    
    def ProcessRegion(self, region):
        # See if it's a gain
        newevents = set()
        if 'gain' not in region.GetHash():
            for event in region.GetEvents():
                if event.runType != 'staging':
                    newevents.add(event)
            region.SetEvents(newevents)
            return

        # Prepare events
        foundEventToProcess = False
        for event in region.GetEvents():
            if event.runType == 'staging':
                foundEventToProcess = True
            
                if event.runNumber and\
                       not self.ftDict.has_key(event.runNumber):
                    f, t = self.getFileTree('tileCalibCIS_%s_CIS.0.root' % event.runNumber, 'h3000')

                    if [f, t] == [None, None]:
                        f, t = self.getFileTree('tileCalibCIS_%s_0.root' % event.runNumber, 'h3000')

                        if [f, t] == [None, None]:
                            f, t = self.getFileTree('tileCalibTool_%s_CIS.0.root' % event.runNumber, 'h3000')
                                                    
                            if [f, t] == [None, None]:
                                if event.runNumber not in self.badRuns:
                                    print "Error: ReadCISFile couldn't load run", event.runNumber, "..."
                                    self.badRuns.add(event.runNumber)
                                continue
                    if hasattr(t, 'nDigitalErrors'):
                        self.ftDict[event.runNumber] = [f, t]
                    #elif event.runNumber not in self.badRuns: 
                    #    print '\tRun %s has old format' % event.runNumber
                    #    self.badRuns.add(event.runNumber) 

        if not foundEventToProcess:
            return

        for event in region.GetEvents():
            if event.runType != 'staging':
                newevents.add(event)
            else:
                # Load up the tree, file
                if not self.ftDict.has_key(event.runNumber):
                    continue 
                [f, t] = self.ftDict[event.runNumber]
                t.GetEntry(0) 
        
                # Get the index of this region within the file
                [x, y, z, w] = region.GetNumber()
                index = self.get_index(x, y - 1, z, w)

                # See if there were even injections
                if int(t.qflag[index]) & 3 != 3:
                    continue

                # Start filling the data for this event
                data = {}
                
                data['calibration'] = t.calib[index]
                
                data['quality_flag'] =  int(t.qflag[index])  #tk remove me temoporary backward compatibility!

#                if event.runNumber < 100489 and\
#                       int(t.qflag[index]) & 7 == 7:
#                    data['isBad'] = False
#                elif event.runNumber >= 100489 and\
#                         int(t.qflag[index]) & 27 == 27:
#                    data['isBad'] = False
#                else:
#                    data['isBad'] = True
                                                                                                                        
                data['nDigitalErrors'] = int(t.nDigitalErrors[index])

                # Define problems for the channel
                problems = {}
                
                data['chi2'] =   t.chi2[index]
                
                # Check for digital errors
                if int(t.qflag[index]) & 64 != 64:
                    assert(data['nDigitalErrors'] != 0)
                    problems['Digital Errors'] = True
                else:
                    problems['Digital Errors'] = False

                # Maximum point in fit range
                if int(t.qflag[index]) & 16 != 16:
                    problems['Fail Max. Point'] = True
                else:
                    problems['Fail Max. Point'] = False

                # Maximum point in fit range
                if int(t.qflag[index]) & 8 != 8:
                    problems['Fail Likely Calib.'] = True
                else:
                    problems['Fail Likely Calib.'] = False
                    
                if not self.turnOffStatCuts:
                    # Large RMS cut
                    if int(t.qflag[index]) & 32 != 32:
                        problems['Large Injection RMS'] = True
                    else:
                        problems['Large Injection RMS'] = False

                    # Chi2
                    if int(t.qflag[index]) & 128 != 128:
                        problems['Low Chi2'] = True
                    else:
                        problems['Low Chi2'] = False
                    
                if int(t.qflag[index]) & 256 != 256:
                    problems['Edge Sample'] = True
                else:
                    problems['Edge Sample'] = False
                    
                if  int(t.qflag[index]) & 512 != 512:
                    problems['Next To Edge Sample'] = True
                else:
                    problems['Next To Edge Sample'] = False
                
                if ('low' in region.GetHash() and t.calib[index] < 0.14) or\
                   ('high' in region.GetHash() and t.calib[index] < 10):
                    problems['No Response'] = True
                else:
                    problems['No Response'] = False

                data['problems'] = problems

                if abs(t.calib[index] - 1.29) > 0.000001 and abs(t.calib[index] - 81.8) > 0.000001:
                    newevents.add(Event('CIS', event.runNumber, data, event.time))

        region.SetEvents(newevents)
