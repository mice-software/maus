# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *

class GetScans(ReadGenericCalibration):
    "Grab the graph(s) of CIS scan(s) from the CIS calibration ntuple"

    def __init__(self, processingDir='/afs/cern.ch/user/t/tilecali/w0/ntuples/cis', getScans=True, getScansRMS=False, all=False):
        self.all = all
        self.processingDir = processingDir
        self.getScans = getScans
        self.getScansRMS = getScansRMS
        self.ftDict = {} # Used for linear constant.  Each element is a [TTree, TFile]
        self.badRuns = set()

    def get_index(self, ros, mod, chan, gain):
        return ros  *64*48*2\
            + mod      *48*2\
            + chan        *2\
            + gain

    def ProcessStart(self):
        self.n = 0

    def ProcessStop(self):
        print 'Grabbed %d scans...' % self.n
    
    def ProcessRegion(self, region):
        # See if it's a gain
        if 'gain' not in region.GetHash():
            return

        # Prepare events
        foundEventToProcess = False
        processRequest = False
        
        for event in region.GetEvents():
            if event.runType == 'CIS':
                foundEventToProcess = True

                if (event.data.has_key('moreInfo') and event.data['moreInfo']) or self.all:
                    processRequest = True
                else:
                    continue
            
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

                    scans = f.Get('cisScans')
                    scans_rms = f.Get('cisScansRMS')
                    if (self.getScans and not scans) or\
                           (self.getScansRMS and not scans_rms) or\
                        (self.getScans and scans.GetName() != 'TMap') or\
                        (self.getScansRMS and scans_rms.GetName() != 'TMap'):
                        if event.runNumber not in self.badRuns:
                            print 'GetScans: Could not find scans. Maybe use ReadOldCISFile?'
                            self.badRuns.add(event.runNumber)
                        continue
                    
                    self.ftDict[event.runNumber] = [f, t, scans, scans_rms]

        if not foundEventToProcess:
            return

        # I used the Upward Lowenheim-Skolem Theorem to determine the validity
        # of this statement.  Actually, not really. :)
        if not processRequest and not self.all:
            return

        # counter
        self.n += 1

        newevents = set()
                
        for event in region.GetEvents():
            if event.runType == 'CIS':
                # Load up the tree, file and scans for this run
                if not self.ftDict.has_key(event.runNumber):
                    continue

                [f, t, scans, scans_rms] = self.ftDict[event.runNumber]
                t.GetEntry(0) 

                x,y,z,w = region.GetNumber()
                key = 'scan%d_%d_%d_%d' % (x, y-1, z, w)
                if self.getScans and scans:
                    obj = scans.GetValue(key)
                    gscan = obj.Clone()
                    gscan.Draw("ALP")
                    #gscan.SetFillColor(0)
                    #gscan.SetLineWidth(6)
                    gscan.GetXaxis().SetTitle("Injected Charge (pC)")
                    gscan.GetXaxis().CenterTitle(True)
                    gscan.GetYaxis().SetTitle("Fit CIS Amplitude (ADC counts)")
                    gscan.SetMarkerSize(1.6)
                    gscan.SetMarkerStyle(20)
                    if gscan.GetFunction("fslope"):
                        gscan.GetFunction("fslope").SetLineWidth(5)
                        gscan.GetFunction("fslope").SetLineColor(2)

                    event.data['scan'] = gscan

                if self.getScansRMS and scans_rms:
                    event.data['scan_rms'] = (scans_rms.GetValue(key)).Clone()
                    
            newevents.add(event)

        region.SetEvents(newevents)
