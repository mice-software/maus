# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 24, 2009
#

from src.ReadGenericCalibration import *
from src.region import *

class CalibDist(ReadGenericCalibration):
    "Plot the distriubiton of calibration constants for both gains"

    def __init__(self):
        self.dir = getPlotDirectory()
        self.dir = '%s/cis' % self.dir
        createDir(self.dir)
        self.c1 = src.MakeCanvas.MakeCanvas()

    def ProcessStart(self):
        self.hvalhi = ROOT.TH1D('hvalhi', '', 10, 74.004, 88.716)
        self.hvallo = ROOT.TH1D('hvallo', '', 10, 1.17796, 1.41004)

    def ProcessStop(self):
        self.c1.SetLogy(0)
        ROOT.gStyle.SetOptStat(111110)
        ROOT.gStyle.SetStatX(0.83)
        ROOT.gStyle.SetStatY(0.83)

        self.c1.Modified()
        self.c1.Update()
        self.c1.Modified()

        self.hvallo.GetXaxis().SetTitle("Low-gain Calibration (ADC counts / pC)")
        self.hvalhi.GetXaxis().SetTitle("High-gain Calibration (ADC counts / pC)")
        for hval in [self.hvallo, self.hvalhi]:
            hval.GetYaxis().SetTitle("Number of ADC Channels")
            hval.GetXaxis().CenterTitle(True)
            hval.SetMaximum(4000)

            hval.Draw()

            # hack
            self.c1.SetLeftMargin(0.14)
            self.c1.SetRightMargin(0.14)
            self.c1.Modified()

            l = ROOT.TLatex()
            l.SetNDC()
            l.SetTextFont(72)
            l.DrawLatex(0.1922,0.867,"ATLAS")

            l2 = ROOT.TLatex()
            l2.SetNDC()
            l2.DrawLatex(0.1922,0.811,"Preliminary")

            self.c1.Print("%s/calib_dist_%s.eps" % (self.dir, hval.GetName()))
            self.c1.Print("%s/calib_dist_%s.png" % (self.dir, hval.GetName()))
            self.c1.Print("%s/calib_dist_%s.root" % (self.dir, hval.GetName()))
        
    def ProcessRegion(self, region):
        meanlo = 0
        nlo = 0

        meanhi = 0
        nhi = 0
        
        for event in region.GetEvents():
            if event.runType == 'CIS':
                variable1 = 'calibratableRegion'
                variable2 = 'calibratableEvent'
                if event.data.has_key('calibration') and event.data.has_key(variable1) and\
                       event.data[variable1] and event.data.has_key(variable2) and\
                       event.data[variable2]:
                    if 'lowgain' in region.GetHash():
                        meanlo += event.data['calibration']
                        nlo += 1
                    else:
                        meanhi += event.data['calibration']
                        nhi += 1


        if nlo != 0:
            self.hvallo.Fill(meanlo/nlo)

        if nhi != 0:
            self.hvalhi.Fill(meanhi/nhi)

                        
                             
