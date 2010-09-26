# ReadChanNoiseFile.py
# Class for reading channel noise from ntuple
# Author: Brian Martin (brian.thomas.martin@cern.ch)
from src.ReadGenericCalibration import *
from src.region import *

class ReadChanNoiseFile(ReadGenericCalibration):
    '''Class to read channel noise values from calibration ntuple'''

    def __init__(self,processingDir='/afs/cern.ch/user/t/tilecali/w0/ntuples/ped'):
        self.processingDir = processingDir
        self.ftDict = {} # Each element is a [TTree, TFile]
        self.load_eflat = False
        self.load_efit  = True
        self.load_eopt  = True
        self.load_edsp  = False
   
    def get_index(self, part, mod, chan, gain):
        return part  *64*48*2\
            + mod      *48*2\
            + chan        *2\
            + gain

    def addTree(self,runNumber):
            f, t = self.getFileTree('RawCh_NoiseCalib_%s_%s.root' % (runNumber,'Ped'), 'RawCh_NoiseCalib')
            if [f, t] == [None, None]:
                f, t = self.getFileTree('RawCh_NoiseCalib_%s_%s.root' % (runNumber,'Phys'), 'RawCh_NoiseCalib')
            if [f, t] == [None, None]:
                print 'File missing for run: ',runNumber
            else:
                self.ftDict[runNumber] = [f, t]

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
                newevents.add(event)
                self.addTree(event.runNumber)
            if event.runType == 'staging':
                if self.addTree(event.runNumber):
                   newevents.add(Event('Ped', event.runNumber, event.data, event.time))
                
        for event in newevents:
            # Get indices 
            [part, mod, chan, gain] = region.GetNumber()
            index = self.get_index(part, mod - 1, chan, gain)
                
            # Get data from noise tree
            [f, t] = self.ftDict[event.runNumber]
            t.GetEntry(0)
            if(self.load_eflat):
                event.data['eflat_av']       =  t.eflat_av[index]
                event.data['eflat_mean']     =  t.eflat_mean[index]
                event.data['eflat_rms']      =  t.eflat_rms[index]
                event.data['eflat_sigma']    =  t.eflat_sigma[index]
                event.data['eflat_kurtosis'] =  t.eflat_kurtosis[index]
            if(self.load_efit):
                event.data['efit_av']        =  t.efit_av[index]
                event.data['efit_mean']      =  t.efit_mean[index]
                event.data['efit_rms']       =  t.efit_rms[index]
                event.data['efit_sigma']     =  t.efit_sigma[index]
                event.data['efit_kurtosis']  =  t.efit_kurtosis[index]
            if(self.load_eopt):
                event.data['eopt_av']        =  t.eopt_av[index]
                event.data['eopt_mean']      =  t.eopt_mean[index]
                event.data['eopt_rms']       =  t.eopt_rms[index]
                event.data['eopt_sigma']     =  t.eopt_sigma[index]
                event.data['eopt_kurtosis']  =  t.eopt_kurtosis[index]
            if(self.load_edsp):
                event.data['edsp_av']        =  t.edsp_av[index]
                event.data['edsp_mean']      =  t.edsp_mean[index]
                event.data['edsp_rms']       =  t.edsp_rms[index]
                event.data['edsp_sigma']     =  t.edsp_sigma[index]
                event.data['edsp_kurtosis']  =  t.edsp_kurtosis[index]
            

        region.SetEvents(newevents)
