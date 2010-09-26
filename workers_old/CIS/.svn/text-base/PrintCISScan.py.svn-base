# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import os, datetime

class PrintCISScan(GenericWorker):
    "Print CIS scans with the 'print' data variable set"

    def __init__(self):
        self.c1 = src.MakeCanvas.MakeCanvas()

        self.dir = getPlotDirectory()
        createDir('%s/print' % self.dir)
        self.dir = '%s/print' % self.dir

    def ProcessRegion(self, region):
        # Only look at each gain within some channel
        if 'gain' not in region.GetHash():
            return

        dumpScan = False

        for event in region.events:
            if event.runType == 'CIS' and event.data.has_key('dumpScan'):
                if event.data['dumpScan']:
                    dumpScan = event.data['dumpScan']

        if not dumpScan:
            return

        self.c1.cd()

        self.c1.Print("%s/uncalib_%s.ps[" % (self.dir, region.GetHash()))

        for event in region.events:
            if event.runType == 'CIS' and event.data.has_key('scan'):
                scan = event.data['scan']
                scan.GetYaxis().SetRangeUser(0.0, 1024.0)
                scan.Draw("ALP")

                tl = ROOT.TLatex()
                tl.SetTextAlign(12)
                tl.SetTextSize(0.04)
                tl.SetNDC()

                cstr = region.GetHash()[8:]
                pstr = region.GetHash(True)[16:19]
                csplit = cstr.split('_')
                newstr = csplit[0] + csplit[1][1:] + ' ' + csplit[2] + '/' + pstr + ' ' + csplit[3]
                
                xoffset = 0.15
                tl.DrawLatex(xoffset,0.95, newstr)
                tl.DrawLatex(xoffset,0.85,"Run %d" % event.runNumber)
                tl.DrawLatex(xoffset,0.80,"Slope This Run: %f" % event.data['calibration'])
                tl.DrawLatex(xoffset,0.75,"DB Slope: %f" % event.data['calibration_db'])
                
                tl.DrawLatex(xoffset,0.70,"DB Variation: %f %s" % (100*(event.data['calibration'] - event.data['calibration_db'])/event.data['calibration_db'], '%'))

                if abs(event.data['deviation']) < 0.01:
                    tl.DrawLatex(xoffset, 0.65,"Pass 1% Flag")
                else:
                    tl.DrawLatex(xoffset, 0.65,"Fail 1% Flag")

                if event.data['quality_flag'] & 16 == 16:
                    tl.DrawLatex(xoffset, 0.60,"Pass Max Point Flag")
                else:
                    tl.DrawLatex(xoffset, 0.60,"Fail Max Point Flag")

                if event.data['quality_flag'] & 8 == 8:
                    tl.DrawLatex(xoffset, 0.55,"Pass Likely Calib Flag")
                else:
                    tl.DrawLatex(xoffset, 0.55,"Fail Likely Calib Flag")

                tl.DrawLatex(xoffset, 0.50,"nDigitalErrors: %d" % int(event.data['nDigitalErrors']))

                if event.data.has_key('numHiRMS'):
                    tl.DrawLatex(xoffset, 0.45,"Points w/ RMS > 5 counts: %d" % int(event.data['numHiRMS']))
                    
                if event.data.has_key('isChi2Hi') and event.data.has_key('isChi2Low'):
                    if event.data['isChi2Low']:
                        tl.DrawLatex(xoffset, 0.40,"High Chi2 Prob.")
                    elif event.data['isChi2Low']:
                        tl.DrawLatex(xoffset, 0.40,"Low Chi2 Prob.")
                    else:
                        tl.DrawLatex(xoffset, 0.40,"Normal Chi2 Prob.")

                self.c1.Print("%s/uncalib_%s.ps" % (self.dir, region.GetHash()))
                
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
                
                                                                                                
        self.c1.Print("%s/uncalib_%s.ps]" % (self.dir, region.GetHash()))
        
        
