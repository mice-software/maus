# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
import src.MakeCanvas

class HistoryPlot(GenericWorker):
    "Compute history plot"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self, runType, savePlot=False):
        self.runType = runType
        self.savePlot = savePlot

        self.dir = getPlotDirectory()

        self.firstTime = True
        self.vals = {}

    def ProcessStart(self):
        if self.firstTime:
            return

        self.hhist = ROOT.TH1F('hhist', '', 100, 100*-0.04, 100*0.04)

        self.c1.SetLogy(1)
        self.hhist.GetXaxis().SetTitle("Time Stability Of Individual Channels (%)")
        self.hhist.GetXaxis().CenterTitle(True)
        

    def ProcessStop(self):
        if self.firstTime:
            self.firstTime = False
            return
            
        self.c1.cd()
        ROOT.gStyle.SetOptStat(111110)
        ROOT.gStyle.SetStatX(0.78)
        ROOT.gStyle.SetStatY(0.83)
        self.hhist.Draw()

        
        # hack
        self.c1.SetLeftMargin(0.14)
        self.c1.SetRightMargin(0.14)
        self.c1.Modified()

        l = ROOT.TLatex()
        l.SetNDC();
        l.SetTextFont(72);
        l.DrawLatex(0.1922,0.867,"ATLAS");
        
        l2 = ROOT.TLatex()
        l2.SetNDC();
        l2.DrawLatex(0.1922,0.811,"Preliminary");

        self.c1.Modified()  
        
        self.c1.Print("%s/history.png" % self.dir)
        self.c1.Print("%s/history.eps" % self.dir)
        self.c1.Print("%s/history.root" % self.dir)
        
        

    def ProcessRegion(self, region):
        if 'gain' not in region.GetHash() or region.GetEvents() == set():
            return

        mean = 0.
        n = 0
        for event in region.GetEvents():
            if event.runType == self.runType:
                if event.data.has_key('calibration') and\
                       event.data.has_key('goodRegion') and\
                       event.data.has_key('goodEvent'):
#                       event.data.has_key('calibratableRegion'):
                    if event.data['goodRegion'] and event.data['goodEvent']:
                        mean += event.data['calibration']
                        n += 1
        if n == 0:
            return
        mean = mean / n
        
        if self.firstTime:
            self.vals[region.GetHash()] = mean
        else:
            if self.vals.has_key(region.GetHash()):
                self.hhist.Fill(100*(mean-self.vals[region.GetHash()])/mean)
                #print 100*(mean-self.vals[region.GetHash()])/mean, mean, self.vals[region.GetHash()]
            else:
                print "couldn't find corresponding for %s" % region.GetHash()

