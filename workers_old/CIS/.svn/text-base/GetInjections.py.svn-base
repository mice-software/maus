# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *
from array import array
from src.MakeCanvas import *

class GetInjections(ReadGenericCalibration):
    "Get the distribution of fit energy for fixed-charges"

    def __init__(self, processingDir='rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/det-tile/2009/', getSamples=True):
        self.processingDir = processingDir
        self.getSamples = getSamples
        self.ftDict = {} # Used for linear constant.  Each element is a [TTree, TFile]
        self.badRuns = set()
        self.c1 = MakeCanvas()

    def ProcessStart(self):
        pass

    def ProcessStop(self):
        pass
    
    def ProcessRegion(self, region):
        # See if it's a gain
        if 'gain' not in region.GetHash():
            return

        dirstr = '%s%02d' % (region.GetHash().split('_')[1], int(region.GetHash().split('_')[2][1:]))

        # Prepare events
        foundEventToProcess = False
        for event in region.GetEvents():
            if event.runType == 'CIS':
                foundEventToProcess = True
            
                if event.runNumber and\
                       not self.ftDict.has_key(event.runNumber):
                    f, t = self.getFileTree('%s/tiletb_%s_CIS.%s.0.aan.root' % (dirstr, event.runNumber, dirstr), 'h1000')

                    if [f, t] == [None, None]:
                        if event.runNumber not in self.badRuns:
                            print "Error: ReadCISFile couldn't load run", event.runNumber, "..."
                            self.badRuns.add(event.runNumber)
                        continue

                    self.ftDict[event.runNumber] = [f, t]

        if not foundEventToProcess:
            return

        map = {'LBA': 'A', 'LBC': 'C', 'EBA': 'D', 'EBC': 'E'}
        if 'lowgain' in region.GetHash():
            gain = 'lo'
        else:
            gain = 'hi'
        module = int(region.GetHash().split('_')[2][1:])
        chan = int(region.GetHash(1).split('_')[3][1:]) - 1
        partition = region.GetHash().split('_')[1]

        hists = [[[] for i in range(33)] for j in range(15)]

        mindac = 0
        minphase = 0
        maxdac = 0
        maxphase = 0
        
        newevents = set()
        for event in region.GetEvents():
            if event.runType == 'CIS':
                # Load up the tree, file and scans for this run
                if not self.ftDict.has_key(event.runNumber):
                    continue 
                [f, t] = self.ftDict[event.runNumber]
                for i in range(t.GetEntries()):
                    if t.GetEntry(i) <= 0:
                        print 'huh?'
                        return

                    if t.m_cispar[7] != 100:
                        continue

                    phase = t.m_cispar[5]
                    dac = t.m_cispar[6]

                    charge = 2.0*4.096*100.0/1023.0 * dac # From TileCisDefaultCalibTool.cxx

                    if gain == 'lo' and not 300 < charge < 700:
                        continue
                    elif gain == 'hi' and not 3 < charge < 10:
                        continue
                                                                                                            
                    e_of_fit = getattr(t, 'efit%s%02d%s' % (map[partition], module, gain))
                    
                    samples = getattr(t, 'sample%s%02d%s' % (map[partition], module, gain))

                    if gain == 'lo':
                        ndac = (dac-384)/32
                    else:
                        ndac = (dac-4)

                    nphase = phase/16

                    if ndac > maxdac:
                        maxdac = ndac

                    if nphase > maxphase:
                        maxphase = nphase

                    if ndac < mindac:
                        mindac = ndac
                            
                    if nphase < minphase:
                        minphase = nphase
                                                                                

                    h = ROOT.TH1F('h_%f_%f_%f' % (charge, phase, e_of_fit[chan]), '', 7, 0, 7)
 
                    for j in range(9):
                        h.Fill(j, samples[chan*7 + j])
 
                    #print nphase, ndac, gain, dac
                    #                    print hists[nphase]
                    #                    print hists[nphase][ndac]
                    hists[nphase][ndac].append(h)

                event.data['samples'] = hists

            newevents.add(event)
                

        region.SetEvents(newevents)

        return
    
        self.c1.Clear()
        ROOT.gStyle.SetPadBorderSize(0)
        ROOT.gStyle.SetFrameBorderSize(0)

        # HG
        #maxdac: 8
        #maxphase: 14

        # LG
        #maxdac: 15
        #maxphase: 14

        x_dac = 16
        y_phase = 16
        self.c1.Divide(x_dac, y_phase, 0.0, 0.0)
        for i in range(maxphase):
            for j in range(maxdac):
                if len(hists[i][j]) != 4:
                    print len(hists[i][j]) , 'ouch', i, j

                self.c1.cd((i+1)*x_dac + (j+1) + 1)
                hists[i][j][0].GetXaxis()
                hists[i][j][0].SetMinimum(0)
                hists[i][j][0].SetMaximum(1023)
                hists[i][j][0].Draw('')
                hists[i][j][1].SetLineColor(ROOT.kRed)
                hists[i][j][1].Draw('SAME')
                hists[i][j][2].SetLineColor(ROOT.kBlue)
                hists[i][j][2].Draw('SAME')
                hists[i][j][3].SetLineColor(ROOT.kGreen)
                hists[i][j][3].Draw('SAME')

        self.c1.Modified()
        self.c1.cd(0)

        tl1 = ROOT.TLatex(0.10, 0.03, "Injected Charge")
        tl2 = ROOT.TLatex(0.03, 0.10, "Phase")
        tl3 = ROOT.TLatex(0.10, 0.97, "Fit-Range Samples for %s" % region.GetHash()[8:])
        
        tl2.SetTextAngle(90)
        for tl in [tl1, tl2, tl3]:
            tl.SetTextAlign(12)
            tl.SetTextSize(0.06)
            tl.SetNDC()
            tl.Draw()

        arrow1 = ROOT.TArrow(0.03,0.3,0.03,0.60,0.02,"|>")
        arrow1.SetAngle(90)
        arrow2 = ROOT.TArrow(0.4,0.03,0.55,0.03,0.02,"|>")

        for arrow in [arrow1, arrow2]:
            arrow.SetLineWidth(1)
            arrow.Draw()
        
        self.c1.Print('samples_%s.pdf' % region.GetHash())
        
        
