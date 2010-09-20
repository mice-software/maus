# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 24, 2009
#

from src.ReadGenericCalibration import *
from src.region import *

class TuneCutsMaxPointLikelyCalib(ReadGenericCalibration):
    "Dump plots for the max point in fit range and likely calib flags"

    def __init__(self):
        self.dir = getPlotDirectory()
        createDir('%s/qflags' % self.dir)
        self.dir = '%s/qflags' % self.dir
        self.c1 = src.MakeCanvas.MakeCanvas()

    def ProcessStart(self):
        self.hmaxvalhi = ROOT.TH1D('hmaxvalhi', '', 100, 0, 1023)
        self.hmaxvallo = ROOT.TH1D('hmaxvallo', '', 100, 0, 1023)

        self.hvalhi = ROOT.TH1D('hvalhi', '', 15, 70., 90.)
        self.hvallo = ROOT.TH1D('hvallo', '', 15, 1.1, 1.4)

    def ProcessStop(self):
        for hmax in [self.hmaxvalhi, self.hmaxvallo]:
            hmax.GetXaxis().SetTitle("Maximum Response in Fit-Range (ADC counts)")
            hmax.GetXaxis().CenterTitle(True)
            self.c1.SetLogy(1)

            # hack
            self.c1.SetLeftMargin(0.14)
            self.c1.SetRightMargin(0.14)
            self.c1.Modified()
                                
            hmax.Draw()

            #leg = ROOT.TLegend(0.241206,0.6573427,0.6243719,0.8706294, "", "brNDC")
            #        (leg.AddEntry(self.hmaxvalhi,"high-gain","f")).SetLineWidth(2)
            #       (leg.AddEntry(self.hmaxvallo,"low-gain","f")).SetLineWidth(2)
            #      leg.Draw()
        
            # hack
            self.c1.SetLeftMargin(0.14)
            self.c1.SetRightMargin(0.14)
            self.c1.Modified()
            
            self.c1.Print("%s/maxval_%s.eps" % (self.dir, hmax.GetName()))
            self.c1.Print("%s/maxval_%s.root" % (self.dir, hmax.GetName()))

        self.c1.SetLogy(0)

        for hval in [self.hvallo, self.hvalhi]:
            hval.GetXaxis().SetTitle("Calibration Factor (ADC counts / pC)")
            hval.GetYaxis().SetTitle("Number of ADC Channels")
            hval.GetXaxis().CenterTitle(True)

            hval.Draw()

            fit = ROOT.TF1("fit%s" % hval.GetName(), "gaus(0)")
            fit.SetParameter(0, hval.GetMaximum())
            fit.SetParameter(1, hval.GetMean())
            fit.SetParameter(2, hval.GetRMS())
            hval.Fit("fit%s" % hval.GetName())

            ptstats = ROOT.TPaveText(0.1809045,0.6713287,0.5050251,0.9213287,"brNDC")
            ptstats.SetName("ptstats%s" % hval.GetName())
            ptstats.SetBorderSize(0)
            ptstats.SetFillColor(0)
            ptstats.SetTextAlign(12)
            ptstats.AddText("#sigma/#mu = %0.1f%s" % (100.*fit.GetParameter(2)/fit.GetParameter(1),'%'))
            ptstats.Draw()
                                            

            # hack
            self.c1.SetLeftMargin(0.14)
            self.c1.SetRightMargin(0.14)
            self.c1.Modified()
                            
            self.c1.Print("%s/calib_dist_%s.eps" % (self.dir, hval.GetName()))
            self.c1.Print("%s/calib_dist_%s.root" % (self.dir, hval.GetName()))
            
            
        """
        self.c1.SetLogy(0)
        self.c1.Print('%s/hprob_stack.ps[' % self.dir)

        hval.SetLineWidth(6)
        self.hrms[0]['hi'].SetLineWidth(6)
        self.hrms[0]['hi'].SetFillColor(1)
        self.hrms[0]['hi'].SetFillStyle(3004)
        hval.SetFillColor(1)
        hval.SetFillStyle(3005)
        #        hval.SetLineColor(ROOT.kRed)
        hist_title = 'RMS of Fit Energy at Fixed-DAC (ADC counts)'
        hval.GetXaxis().SetTitle(hist_title)

        hval.GetXaxis().SetNdivisions(509)
        hval.GetXaxis().SetLabelSize(0.06)
        hval.GetXaxis().SetLabelOffset(0.01)
        hval.GetXaxis().SetTitleSize(0.07)
        hval.GetYaxis().SetNdivisions(305)
        hval.GetYaxis().SetLabelSize(0.06)
        hval.GetXaxis().CenterTitle(True)
        hval.GetXaxis().SetTitleOffset(1.03)
                        
        
#        self.hrms[0]['hi'].SetLineColor(ROOT.kBlue)
        if hval.GetEntries():
            hval.Draw()
            self.hrms[0]['hi'].Draw('SAME')
        elif self.hrms[0]['hi'].GetEntries():
            self.hrms[0]['hi'].Draw()
            
        leg = ROOT.TLegend(0.508794,0.708042,0.8919598,0.9213287,"","brNDC")
        leg.SetBorderSize(0)
        leg.SetFillColor(0)
        (leg.AddEntry(self.hrms[0]['hi'],"high-gain","f")).SetLineWidth(2)
        (leg.AddEntry(hval,"low-gain","f")).SetLineWidth(2)
        leg.Draw()

        self.c1.Print('%s/hprob_stack.ps' % self.dir)
        self.c1.Print('%s/hprob_stack_1.eps' % self.dir)
        self.c1.Print('%s/hprob_stack_1.root' % self.dir)

        self.c1.SetLogy(1)

        self.hrms[1]['hi'].SetFillColor(1)
        self.hrms[1]['hi'].SetFillStyle(3004)
        self.hrms[1]['lo'].SetFillColor(1)
        self.hrms[1]['lo'].SetFillStyle(3005)
        self.hrms[1]['lo'].SetLineWidth(6)
        self.hrms[1]['hi'].SetLineWidth(6)
        #        self.hrms[1]['lo'].SetLineColor(ROOT.kRed)
        self.hrms[1]['lo'].GetXaxis().SetTitle(hist_title)
        #        self.hrms[1]['hi'].SetLineColor(ROOT.kBlue)
        self.hrms[1]['lo'].GetXaxis().SetNdivisions(509)
        self.hrms[1]['lo'].GetXaxis().SetLabelSize(0.06)
        self.hrms[1]['lo'].GetXaxis().SetLabelOffset(0.01)
        self.hrms[1]['lo'].GetXaxis().SetTitleSize(0.07)
        self.hrms[1]['lo'].GetYaxis().SetNdivisions(305)
        self.hrms[1]['lo'].GetYaxis().SetLabelSize(0.06)

        self.hrms[1]['lo'].GetXaxis().CenterTitle(True)
        self.hrms[1]['lo'].GetXaxis().SetTitleOffset(1.03)
        

        if self.hrms[1]['lo'].GetEntries():
            self.hrms[1]['lo'].Draw()
            self.hrms[1]['hi'].Draw('SAME')
        elif self.hrms[1]['hi'].GetEntries():
            self.hrms[1]['hi'].Draw()
        
        leg = ROOT.TLegend(0.508794,0.708042,0.8919598,0.9213287,"","brNDC")
        leg.SetBorderSize(0)
        leg.SetFillColor(0)
        (leg.AddEntry(self.hrms[1]['hi'],"high-gain","f")).SetLineWidth(2)
        (leg.AddEntry(self.hrms[1]['lo'],"low-gain","f")).SetLineWidth(2)
        leg.Draw()
        
        self.c1.Print('%s/hprob_stack.ps' % self.dir)
        self.c1.Print('%s/hprob_stack_2.eps' % self.dir)
        self.c1.Print('%s/hprob_stack_2.root' % self.dir)
        

        self.c1.SetLogy(0)

        i = 3
        for hbad in [self.hprobbad,\
                     self.hprobbadlowzoom,\
                     self.hprobbadhighzoom]:
            hbad.SetLineWidth(6)
            hbad.SetFillColor(1)
            hbad.SetFillStyle(3002)

            hbad.GetXaxis().SetTitle('Probability of \chi^{2} and NDF')
            hbad.GetXaxis().SetNdivisions(509)
            hbad.GetXaxis().SetLabelSize(0.06)
            hbad.GetXaxis().SetLabelOffset(0.01)
            hbad.GetXaxis().SetTitleSize(0.07)
            hbad.GetXaxis().CenterTitle(True)
            hbad.GetXaxis().SetTitleOffset(1.03);
            hbad.GetYaxis().SetLabelSize(0.06)
            
            hbad.Draw()

            self.c1.Modified()
            self.c1.Update()
            self.c1.Print('%s/hprob_stack.ps' % self.dir)
            self.c1.Print('%s/hprob_stack_%d.eps' % (self.dir, i))
            self.c1.Print('%s/hprob_stack_%d.root' % (self.dir, i))
            
            i += 1

        self.c1.Print('%s/hprob_stack.ps]' % self.dir)
        """
        
    def ProcessRegion(self, region):

        done = False
        for event in region.GetEvents():
            if event.runType == 'CIS':
                if event.data.has_key('calibration') and event.data.has_key('goodRegion') and\
                       event.data['goodRegion'] and not done:
                    if 'lowgain' in region.GetHash():
                        self.hvallo.Fill(event.data['calibration'])
                    else:
                        self.hvalhi.Fill(event.data['calibration'])
                    done = True
                        
                if event.data.has_key('scan') and event.data['scan']:
                    gscan = event.data['scan']

                    maxpoint = -9999
                    for i in range(gscan.GetN()):
                        if 'high' in region.GetHash() and not 3.<gscan.GetX()[i]<10.:
                            pass
                        elif 'low' in region.GetHash() and not 300.<gscan.GetX()[i]<700.:
                            pass
                        else:
                            if gscan.GetY()[i] > maxpoint:
                                maxpoint = gscan.GetY()[i]
                                
                                

                    if 'high' in region.GetHash():
                        self.hmaxvalhi.Fill(maxpoint)
                    else:
                        self.hmaxvallo.Fill(maxpoint)
                        
                    
                        
                        
                             
