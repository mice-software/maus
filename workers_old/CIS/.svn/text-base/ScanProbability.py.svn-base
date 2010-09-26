# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 24, 2009
#

from src.ReadGenericCalibration import *
from src.region import *

class ScanProbability(ReadGenericCalibration):
    "Compute the linear fit probability of a CIS scan"

    def __init__(self):
        self.dir = getPlotDirectory()
        createDir('%s/prob' % self.dir)
        self.dir = '%s/prob' % self.dir
        self.c1 = src.MakeCanvas.MakeCanvas()

    def ProcessStart(self):
        self.hprobgood = ROOT.TH1D('hprobgood', '', 100, 0, 1)
        self.hprobbad = ROOT.TH1D('hprobbad', '', 100, 0, 1)

        self.hprobgoodlowzoom = ROOT.TH1D('hprobgoodlowzoom', '', 30, 0, 0.00002)
        self.hprobbadlowzoom = ROOT.TH1D('hprobbadlowzoom', '', 30, 0, 0.00002)

        self.hprobgoodhighzoom = ROOT.TH1D('hprobgoodhighzoom', '', 30, 0.9996, 1)
        self.hprobbadhighzoom = ROOT.TH1D('hprobbadhighzoom', '', 30, 0.9996, 1)

        self.hrms = [{'lo': ROOT.TH1D("hrmslo", "", 50, 0, 10), 'hi': ROOT.TH1D("hrmshi", "", 50, 0, 10)},
                     {'lo': ROOT.TH1D("hrmslobig", "", 500, 0, 200), 'hi': ROOT.TH1D("hrmshibig", "", 500, 0, 200)}]
        

    def ProcessStop(self):
        self.c1.SetLogy(0)
        self.c1.Print('%s/hprob_stack.ps[' % self.dir)

        self.hrms[0]['lo'].SetLineColor(ROOT.kRed)
        self.hrms[0]['lo'].GetXaxis().SetTitle('RMS of fixed-DAC floating-phase (ADC counts)')
        self.hrms[0]['hi'].SetLineColor(ROOT.kBlue)
        if self.hrms[0]['lo'].GetEntries():
            self.hrms[0]['lo'].Draw()
            self.hrms[0]['hi'].Draw('SAME')
        elif self.hrms[0]['hi'].GetEntries():
            self.hrms[0]['hi'].Draw()
            
        leg = ROOT.TLegend(0.5100503,0.6066434,0.7148241,0.7447552, "","brNDC")
        leg.SetBorderSize(0)
        leg.SetFillColor(0)
        leg.AddEntry(self.hrms[0]['hi'],"high-gain","l")
        leg.AddEntry(self.hrms[0]['lo'],"low-gain","l")
        leg.Draw()

        self.c1.Print('%s/hprob_stack.ps' % self.dir)

        self.c1.SetLogy(1)

        self.hrms[1]['lo'].SetLineColor(ROOT.kRed)
        self.hrms[1]['lo'].GetXaxis().SetTitle('RMS of fixed-DAC floating-phase (ADC counts)')
        self.hrms[1]['hi'].SetLineColor(ROOT.kBlue)
        if self.hrms[1]['lo'].GetEntries():
            self.hrms[1]['lo'].Draw()
            self.hrms[1]['hi'].Draw('SAME')
        elif self.hrms[1]['hi'].GetEntries():
            self.hrms[1]['hi'].Draw()
        
        leg = ROOT.TLegend(0.5100503,0.6066434,0.7148241,0.7447552, "","brNDC")
        leg.SetBorderSize(0)
        leg.SetFillColor(0)
        leg.AddEntry(self.hrms[1]['hi'],"high-gain","l")
        leg.AddEntry(self.hrms[1]['lo'],"low-gain","l")
        leg.Draw()
        
        self.c1.Print('%s/hprob_stack.ps' % self.dir)

        self.c1.SetLogy(0)
        
        for hgood, hbad in [(self.hprobgood, self.hprobbad),\
                            (self.hprobgoodlowzoom,self.hprobbadlowzoom),\
                            (self.hprobgoodhighzoom,self.hprobbadhighzoom)]:
            hs = ROOT.THStack('hs', '')
            hbad.SetFillColor(ROOT.kRed)
            hgood.SetFillColor(ROOT.kGreen)
            if hgood.GetEntries():
                hs.Add(hgood)
            if hbad.GetEntries():
                hs.Add(hbad)
            hbad.GetXaxis().SetTitle('Probability')
            hgood.GetXaxis().SetTitle('Probability')
            if hgood.GetEntries() or hbad.GetEntries():
                hs.Draw()

            leg = ROOT.TLegend(0.5100503,0.7066434,0.7148241,0.8447552, "","brNDC")
            leg.SetBorderSize(0)
            leg.SetFillColor(0)
            leg.AddEntry(hgood,"Problems","f")
            leg.AddEntry(hbad,"No Problems","f")
            leg.Draw()
            
            self.c1.Modified()
            self.c1.Update()
            self.c1.Print('%s/hprob_stack.ps' % self.dir)

        self.c1.Print('%s/hprob_stack.ps]' % self.dir)
    
    def ProcessRegion(self, region):
        newevents = set()

        isChi2hi = False
        isChi2lo = False

        for event in region.GetEvents():
            if event.runType == 'CIS':
                numHiRMS = 0
                        
                if event.data.has_key('scan') and event.data['scan'] and\
                       event.data.has_key('scan_rms') and event.data['scan_rms']:
                    gscan = event.data['scan']
                    gscan_rms = event.data['scan_rms']

                    for i in range(gscan.GetN()):
                        if 'high' in region.GetHash() and not 3.<gscan.GetX()[i]<10.:
                            pass
                        elif 'low' in region.GetHash() and not 300.<gscan.GetX()[i]<700.:
                            pass
                        else:
                            for dict in self.hrms:
                                if 'high' in region.GetHash():
                                    dict['hi'].Fill(gscan_rms.GetErrorY(i))
                                else:
                                    dict['lo'].Fill(gscan_rms.GetErrorY(i))

                        delta = 0.5
                        gscan.SetPointError(i, 0, math.sqrt(gscan.GetErrorY(i)*gscan.GetErrorY(i) + delta*delta))
                        
                    fit = None
                    fit2 = None
                    if gscan.GetFunction('fslope'):
                        fit2 = gscan.GetFunction('fslope').Clone()

                    function = '[0]*x'
                    #function = '[0]*x + [1]'
                    if 'high' in region.GetHash():
                        fit = ROOT.TF1('fit', '[0]*x', 3.0, 10.0)
                        fit.SetParameter(0, 81.8)
                    else:
                        fit = ROOT.TF1('fit', '[0]*x', 300.0, 700.0)
                        fit.SetParameter(0, 1.29)

                    if '[1]' in function:
                        fit.SetParameter(1, 0)

                    gscan.Fit(fit, 'qr')

                    mychi2 = 0
                    for i in range(gscan.GetN()):
                         if 'high' in region.GetHash() and not 3.<gscan.GetX()[i]<10.:
                             continue
                         elif 'low' in region.GetHash() and not 300.<gscan.GetX()[i]<700.:
                             continue

                         if gscan_rms.GetErrorY(i) > 5:
                             numHiRMS += 1

                             #print 'x', region.GetHash(), region.GetHash(1), gscan.GetX()[i]/(2.0*4.096*100.0/1023.0), gscan_rms.GetErrorY(i)

                         if gscan.GetErrorY(i) == 0.:
                             print x,y,z,w,i, gscan.GetX()[i], gscan.GetErrorY(i)
                         else:
                             mychi2 += (gscan.GetY()[i] - fit.Eval(gscan.GetX()[i]))*(gscan.GetY()[i] - fit.Eval(gscan.GetX()[i]))/gscan.GetErrorY(i)/gscan.GetErrorY(i)


                    chi2 = fit.GetChisquare()
                    if not event.data['goodEvent']:
                        for h in [self.hprobbad, self.hprobbadlowzoom, self.hprobbadhighzoom]:
                            h.Fill(ROOT.TMath.Prob(fit.GetChisquare(), fit.GetNDF()))
                    else:
                        for h in [self.hprobgood, self.hprobgoodhighzoom, self.hprobgoodlowzoom]:
                            h.Fill(ROOT.TMath.Prob(fit.GetChisquare(), fit.GetNDF()))

                    if ROOT.TMath.Prob(fit.GetChisquare(), fit.GetNDF()) < 2*10**-6:
                        isChi2lo = True

                    if ROOT.TMath.Prob(fit.GetChisquare(), fit.GetNDF()) > 0.9999:
                        isChi2hi = True

            problems = event.data['problems']

            if numHiRMS != 0:
                problems['Large Injection RMS'] = True
                event.data['goodEvent'] = False
                event.data['calibratableEvent'] = False
            else:
                problems['Large Injection RMS'] = False
                
            if isChi2lo:
                problems['Low Chi2'] = True
                event.data['goodEvent'] = False
                event.data['calibratableEvent'] = False
            else:
                problems['Low Chi2'] = False
                                                
            if isChi2hi:
                problems['High Chi2'] = True
                event.data['goodEvent'] = False
                event.data['calibratableEvent'] = False
            else:
                problems['High Chi2'] = False

            event.data['problems'] = problems
                                 
            newevents.add(event)

        region.SetEvents(newevents)
                    
                        
                        
                             
