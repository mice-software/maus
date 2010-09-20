# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *
from array import array
from src.MakeCanvas import *

class GetSamples(ReadGenericCalibration):
    "Get the samples for a scan and the distribution of fit energy for fixed-charges"

    def __init__(self, processingDir='rfio:/castor/cern.ch/grid/atlas/atlasgroupdisk/det-tile/2009/',\
                 getSamples=True, getInjDist=True, all=False):
        self.processingDir = processingDir
        self.getSamples = getSamples
        self.getInjDist = getInjDist
        self.all=all
        self.ftDict = {} # Used for linear constant.  Each element is a [TTree, TFile]
        self.badRuns = set()
        self.dir = getPlotDirectory() + '/cis/samples'
        createDir(self.dir) 
        self.c1 = MakeCanvas()
        self.c1.SetHighLightColor(2)
        self.h_index = 0

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
                if not event.data.has_key('moreInfo') and not self.all:
                    continue
                
                if not self.all and not event.data['moreInfo']:
                    continue
                
                foundEventToProcess = True
            
                if event.runNumber and\
                       not self.ftDict.has_key(event.runNumber):
                    #print '%s%s/tiletb_%s_CIS.%s.0.aan.root' % (self.processingDir, dirstr, event.runNumber, dirstr)
                    f, t = self.getFileTree('%s/tiletb_%s_CIS.%s.0.aan.root' % (dirstr, event.runNumber, dirstr), 'h1000')

                    if [f, t] == [None, None]:
                        if event.runNumber not in self.badRuns:
                            print "Error: GetSamples couldn't load run", event.runNumber, ' for %s...' % dirstr
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

        newevents = set()
        for event in region.GetEvents():
            hists = [[[] for i in range(16)] for j in range(15)]

            maxdac = 0
            maxphase = 0
            
            if event.runType == 'CIS':
                if not self.all and \
                       (event.data.has_key('moreInfo') and not event.data['moreInfo']):
                    continue
                # Load up the tree, file and scans for this run
                if not self.ftDict.has_key(event.runNumber):
                    continue 
                [f, t] = self.ftDict[event.runNumber]

                injections = {}
                
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
                    
                    if not hasattr(t, 'efit%s%02d%s' % (map[partition], module, gain)) or\
                       not hasattr(t, 'tfit%s%02d%s' % (map[partition], module, gain)) or\
                       not hasattr(t, 'pedfit%s%02d%s' % (map[partition], module, gain)) or\
                       not hasattr(t, 'sample%s%02d%s' % (map[partition], module, gain)):
                        return
                    
                    e_of_fit = getattr(t, 'efit%s%02d%s' % (map[partition], module, gain))
                    t_of_fit = getattr(t, 'tfit%s%02d%s' % (map[partition], module, gain))
                    p_of_fit = getattr(t, 'pedfit%s%02d%s' % (map[partition], module, gain))
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

                    h = ROOT.TH1F('h_%f_%f_%f_%d' % (charge, phase, e_of_fit[chan], self.h_index), '', 7, 0, 7)
                    self.h_index += 1
                    
                    #                    print dac, phase, 
                    for j in range(7):
                        h.Fill(j, samples[chan*9 + j])

                    #                   h.Draw()


                    # this is a mess.
                    self.c1.cd()
                    h.GetXaxis().SetTitle("Sample Number")
                    h.GetYaxis().SetTitle("ADC counts")
                    h.GetXaxis().SetTitleOffset(0.93)
                    h.GetXaxis().SetTitleSize(0.05)
                    h.GetYaxis().SetTitleSize(0.05)
                    h.GetYaxis().SetTitleOffset(0.99)
                    h.SetMinimum(0)
                    h.SetMaximum(1023)
                    #h.SetFillColor(1)
                    #h.SetFillStyle(3002)
                    #h.SetLineWidth(6)
                    h.Draw()

                    tl1 = ROOT.TLatex(0.10, 0.92, "Fit Energy: %f" % e_of_fit[chan])
                    tl2 = ROOT.TLatex(0.53, 0.92, "Fit Time: %f" % t_of_fit[chan])
                    tl3 = ROOT.TLatex(0.40, 0.84, "Fit Pedestal: %f" % p_of_fit[chan])

                    for tl in [tl1, tl2, tl3]:
                        tl.SetNDC()
                        tl.SetTextSize(0.06)
                        tl.Draw()

                    #self.c1.Print("%s/sample_%s_%f_%f_%f.ps" % (self.dir, region.GetHash(), charge, phase, e_of_fit[chan]))
                    #self.c1.Print("%s/sample_%s_%f_%f_%f.root" % (self.dir, region.GetHash(), charge, phase, e_of_fit[chan]))
                        #print samples[chan*9 + j],
                        #                    print
 
                    #print nphase, ndac, gain, dac
                    #                    print hists[nphase]
                    #                    print hists[nphase][ndac]
                    hists[nphase][ndac].append(h)

                    if not injections.has_key(charge):
                        injections[charge] = []
                    injections[charge].append(e_of_fit[chan])

                event.data['samples'] = hists

            inj_hists = {}
            for z, w in  injections.iteritems(): # z charge, w arrany
                dx = max([(max(w) - min(w))*0.1, 2.5])
                
                h = ROOT.TH1F('%s_%f_%d' % (region.GetHash(), z, event.runNumber), '', 10, min(w)-dx, max(w)+dx)
                for n in w:
                    h.Fill(n)
                    
                inj_hists[z] = h

            event.data['injections'] = inj_hists
            event.data['maxphase'] = maxphase
            event.data['maxdac'] = maxdac
            newevents.add(event)
                

        region.SetEvents(newevents)


#        arrow1 = ROOT.TArrow(0.03,0.3,0.03,0.60,0.02,"|>")
#        arrow1.SetAngle(90)
#        arrow2 = ROOT.TArrow(0.4,0.03,0.55,0.03,0.02,"|>")

#        for arrow in [arrow1, arrow2]:
#            arrow.SetLineWidth(1)
#            arrow.Draw()
         
        
