# ReadDigiNoiseFile.py
# Class for reading digital noise from ntuple
# Author: Brian Martin (brian.thomas.martin@cern.ch)
from src.ReadGenericCalibration import *
from src.region import *

class ReadDigiNoiseFile(NoiseWorker,ReadGenericCalibration):
    '''Class to read digital noise values from calibration ntuple'''

    def __init__(self,processingDir='/afs/cern.ch/user/t/tilecali/w0/ntuples/ped',load_autocr=True):
        self.processingDir = processingDir
        self.ftDict = {} # Each element is a [TTree, TFile]
        self.load_autocr = load_autocr
    
    def addTree(self,runNumber):
            f, t = self.getFileTree('Digi_NoiseCalib_%s_%s.root' % (runNumber,'Ped'), 'Digi_NoiseCalib')
            if [f, t] == [None, None]:
                f, t = self.getFileTree('Digi_NoiseCalib_%s_%s.root' % (runNumber,'Phys'), 'Digi_NoiseCalib')
            if [f, t] == [None, None]:
                print 'File missing for run: ',runNumber
                return False
            else:
                self.ftDict[runNumber] = [f, t]

            return True

    def ProcessStart(self):
        pass
        
    def ProcessStop(self):
        pass
        
    def ProcessRegion(self, region):
        newevents = set()
        # only interested in adc level
        if 'gain' not in region.GetHash():
            for event in region.GetEvents():
                if event.runType != 'staging':
                    newevents.add(event)
            region.SetEvents(newevents)
            return
        
        # check if Ped event exists (if so, use them). if staging events exist, convert to ped
        for event in region.GetEvents():
            if event.runType == 'Ped':
                if self.addTree(event.runNumber):
                    newevents.add(event)
            if event.runType == 'staging':
                if self.addTree(event.runNumber):
                    newevents.add(Event('Ped', event.runNumber, event.data, event.time))
                
        for event in newevents:
            # Get indices 
            [part, mod, chan, gain] = region.GetNumber()
            index = self.get_index(region.GetType(),part, mod - 1, chan, gain)
            # Get data from noise tree
            [f, t] = self.ftDict[event.runNumber]
            t.GetEntry(0) 
            event.data['ped'] =  t.ped[index]
            event.data['hfn'] =  t.hfn[index]
            event.data['lfn'] =  t.lfn[index]
            if self.load_autocr:
                for x in xrange(6):
                    event.data['autocorr'+str(x)] = t.auto_corr[index+x]

        region.SetEvents(newevents)
