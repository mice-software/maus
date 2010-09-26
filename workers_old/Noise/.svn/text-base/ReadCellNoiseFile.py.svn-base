# ReadCellNoiseFile.py
# Class for reading cell noise from ntuple
# Author: Brian Martin (brian.thomas.martin@cern.ch)
from src.ReadGenericCalibration import *
from src.region import *

class ReadCellNoiseFile(ReadGenericCalibration):
    '''Class to read cell noise values from calibration ntuple'''

    def __init__(self,processingDir='/afs/cern.ch/user/t/tilecali/w0/ntuples/ped'):
        self.processingDir = processingDir
        self.ftDict = {} # Each element is a [TTree, TFile]
        self.type='physical'
        self.gainStr = ['LGLG','LGHG','HGLG','HGHG','HG--','--HG']
        self.part2side = [-1,0,1,0,1] # [AUX,LBA,LBC,EBA,EBC]
        
    def get_index(self, side, mod, samp, tower, gain):
        return side *64*4*17*6\
            + mod      *4*17*6\
            + samp       *17*6\
            + tower         *6\
            + gain

    def addTree(self,runNumber):
            f, t = self.getFileTree('RawCh_NoiseCalib_%s_%s.root' % (runNumber,'Ped'), 'RawCh_NoiseCalib')
            if [f, t] == [None, None]:
                f, t = self.getFileTree('RawCh_NoiseCalib_%s_%s.root' % (runNumber,'Phys'), 'RawCh_NoiseCalib')
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
        # only interested in cell level
        if '_t' not in region.GetHash():
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
            # Get data from noise tree
            [f, t] = self.ftDict[event.runNumber]
            t.GetEntry(0) # ntuples have run info stored as single event
            # Get indices 
            [part, mod, samp, tower] = region.GetNumber()
            for gain in xrange(6):
                index = self.get_index(self.part2side[part], mod - 1, samp, tower, gain)
                event.data['cellenergy'+self.gainStr[gain]]  =  t.ecell_av[index]
                event.data['cellnoise'+self.gainStr[gain]]   =  t.ecell_rms[index]
           
        region.SetEvents(newevents)
