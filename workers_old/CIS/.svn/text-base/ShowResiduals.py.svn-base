# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import os, datetime

class ShowResiduals(GenericWorker):
    "Make a plot of the CIS fit residuals"

    def __init__(self):
        self.c1 = src.MakeCanvas.MakeCanvas()

        self.dir = getPlotDirectory() + "/cis"
        createDir('%s/residuals' % self.dir)
        self.dir = '%s/residuals' % self.dir

    def ProcessRegion(self, region):
        # Only look at each gain within some channel
        if 'gain' not in region.GetHash():
            return

        self.c1.cd()

        for event in region.GetEvents():
            if event.runType == 'CIS' and event.data.has_key('scan'):
                scan = event.data['scan']
                gnln = ROOT.TGraphErrors()
                gnlni = 0
                for i in range(scan.GetN()):
                    if 'high' in region.GetHash() and not 3.<scan.GetX()[i]<10.:
                        continue
                    elif 'low' in region.GetHash() and not 300.<scan.GetX()[i]<700.:
                        continue

                    gnln.SetPoint(gnlni, scan.GetX()[i], scan.GetY()[i] - event.data['calibration']*scan.GetX()[i])
                    gnln.SetPointError(gnlni, 0, scan.GetErrorY(i))
                    gnlni += 1

                gnln.Draw("ALP")
                gnln.SetFillColor(0)
                gnln.GetXaxis().SetTitle("Injected Charge (pC)")
                gnln.GetYaxis().SetTitle("Data - Fit (ADC counts)")
                gnln.GetYaxis().SetLabelSize(0.05)
                gnln.GetYaxis().SetTitleSize(0.05)
                gnln.GetXaxis().SetLabelOffset(0.02)
                gnln.GetXaxis().SetLabelOffset(0.01)
                gnln.GetXaxis().SetLabelSize(0.05)
                gnln.GetXaxis().SetTitleSize(0.05)
                gnln.GetXaxis().SetTitleOffset(0.93)
                gnln.SetLineWidth(3)
                gnln.SetMarkerSize(1.6)
                gnln.SetMarkerStyle(20)
                gnln.Draw('ALP')
                self.c1.Print("%s/uncalib_%s.ps" % (self.dir, region.GetHash()))
                
                                                                                                
        
        
