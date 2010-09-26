# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import time
import datetime
from math import *

class TimeVSMeanCalib(GenericWorker):
    "Compute the time stability of a calibration constant"

    def __init__(self, runType = 'CIS', parameter='calibration', example='EBC_m10_c14', syst=0.007):
        self.runType = runType
        self.parameter = parameter
        self.example = example
        self.syst= syst

        self.dir = getPlotDirectory() + '/cis'
        createDir(self.dir)

        self.c1 = src.MakeCanvas.MakeCanvas()

        self.factor = 60*60*24

    def ProcessStart(self):
        #self.glo         = ROOT.TGraph()
        #self.glo_example = ROOT.TGraph()
        #self.ghi         = ROOT.TGraph()
        #self.ghi_example = ROOT.TGraph()

        self.lo = {}
        self.hi = {}

        self.lo_example = {}
        self.hi_example = {}

    def ProcessStop(self):
        #da = ROOT.TDatime(1970,01,01,12,00,00);
        #ROOT.gStyle.SetTimeOffset(da.Convert())
        ROOT.gStyle.SetNdivisions(304, "y")
        self.c1.Modified()
        
        for all, example, gain in [(self.lo, self.lo_example, 0), (self.hi, self.hi_example, 1)]:
            gall     = ROOT.TGraphErrors()
            gall_axis = ROOT.TGraphErrors()
            gall_sys = ROOT.TGraphErrors()
            
            gall_sys.SetFillStyle(3004)

            i = 0
            a = all.keys()
            a.sort()
            for time in a:
                calibs = all[time]
                mean = ROOT.TMath.Mean(len(calibs), array('f', calibs))
                rms = ROOT.TMath.RMS(len(calibs), array('f', calibs))

                gall.SetPoint(i, time, mean)
                gall_axis.SetPoint(i, time, mean)
                gall_sys.SetPoint(i, time, mean)
                gall.SetPointError(i, 0, rms/sqrt(len(calibs)))
                gall_sys.SetPointError(i, 0, self.syst*mean)
                i += 1


            gexample = ROOT.TGraphErrors()
            gexample.SetMarkerStyle(23)
            gexample.SetMarkerColor(2)
            gexample_sys = ROOT.TGraphErrors()
            gexample_sys.SetLineStyle(2)
            gexample_sys.SetLineWidth(4)
            #gexample_sys.SetFillColor(ROOT.kGray)
            #gexample_sys.SetFillStyle(1001)

            i=0
            a = example.keys()
            a.sort()

            i=0
            for time in a:
                calibs = example[time]
                mean = ROOT.TMath.Mean(len(calibs), array('f', calibs))
                rms = ROOT.TMath.RMS(len(calibs), array('f', calibs))
                
                gexample.SetPoint(i, time, mean)
                gexample.SetPointError(i, 0, rms/sqrt(len(calibs)))
                i += 1

            #gall_axis.Draw("ALP")
            #self.c1.Update()
            #self.c1.Modified()
            #i=0
            #print self.c1.GetUxmin(), self.c1.GetUxmax()
            #gexample_sys.SetPoint(0, self.c1.GetUxmin(), gexample.GetMean(2))
            #gexample_sys.SetPointError(0, 0, self.syst*gexample.GetMean(2))
            #gexample_sys.SetPoint(1, self.c1.GetUxmax(), gexample.GetMean(2))
            #gexample_sys.SetPointError(1, 0, self.syst*gexample.GetMean(2))
            #for time in a:
            #    gexample_sys.SetPoint(i, time, gexample.GetMean(2))
            #    gexample_sys.SetPointError(i, 0, self.syst*gexample.GetMean(2))
            #    i += 1
                
            gall_axis.Draw("AP")
            gall_sys.Draw("A3")
            gexample_sys.Draw("A3")
            gall.Draw("P")
            gexample.Draw("P")

            if gain ==  0:
                gall_axis.GetHistogram().GetYaxis().SetTitle("Low gain Calibration (ADC counts/pC)")
            else:
                gall_axis.GetHistogram().GetYaxis().SetTitle("High gain Calibration (ADC counts/pC)")
            gall_axis.GetHistogram().GetXaxis().SetTitle("Time (months)")
            gall_axis.SetMaximum(gall.GetMean(2)*1.035)
            gall_axis.SetMinimum(gall.GetMean(2)*0.975)
            gall_axis.GetHistogram().GetXaxis().SetLabelOffset(1.)
            gall_axis.Draw("AP")
            haxis = gall_axis.GetHistogram()

            ax = gall_sys.GetHistogram().GetXaxis()

            da = ROOT.TDatime(2008, 8,01,12,00,00).Convert()/self.factor
            ax.SetBinLabel(ax.FindBin(da), "1 Aug" )
                
            da = ROOT.TDatime(2008, 9,01,12,00,00).Convert()/self.factor
            ax.SetBinLabel(ax.FindBin(da), "1 Sept" )

            da = ROOT.TDatime(2008,10,01,12,00,00).Convert()/self.factor
            ax.SetBinLabel(ax.FindBin(da), "1 Oct" )

            da = ROOT.TDatime(2008,11,01,12,00,00).Convert()/self.factor
            ax.SetBinLabel(ax.FindBin(da), "1 Nov" )

            ax.LabelsOption("h")
            ax.SetLabelSize(0.05)

            #ax = gall_sys.GetHistogram().GetYaxis()
             
            #ax.SetBinLabel(ax.FindBin(80), "80.0" )
            #ax.SetBinLabel(ax.FindBin(82), "82.0" )
            #ax.SetBinLabel(ax.FindBin(84), "84.0" )

            gall_sys.Draw("ALP")

            self.c1.Clear()
            gall_sys.GetHistogram().GetYaxis().SetDecimals()
            haxis.GetYaxis().SetDecimals()
            haxis.GetXaxis().SetRangeUser(ROOT.TDatime(2008, 8,01,12,00,00).Convert()/self.factor,\
                                          ROOT.TDatime(2008,11,01,12,00,00).Convert()/self.factor)
            haxis.GetXaxis().SetNdivisions(503,0)
            haxis.Draw("AXIS")
            gall_sys.GetHistogram().Draw("AXISSAME")
            self.c1.Modified()
            ##gall_sys.Draw("3")  # too busy with this
            #tpave2 = ROOT.TBox()
            #tpave2.SetFillColor(ROOT.kGray)
            #tpave2.SetFillStyle(1001)
            #tpave2.SetLineColor(0)
            #tpave2.SetBorderSize(0)
            #print self.c1.GetUxmin(),(1-self.syst)*gexample.GetMean(2),self.c1.GetUxmax(),(1+self.syst)*gexample.GetMean(2)
            #14091.476 81.4252918254 14184.392 
            #tpave2.DrawBox(14091.476, (1-self.syst)*gexample.GetMean(2),14184.392,(1+self.syst)*gexample.GetMean(2))

            line = ROOT.TLine()
            line.SetLineStyle(2)
            line.SetLineWidth(4)
            line.DrawLine(14091.476, (1+self.syst)*gexample.GetMean(2), 14184.392, (1+self.syst)*gexample.GetMean(2))
            line.DrawLine(14091.476, (1-self.syst)*gexample.GetMean(2), 14184.392, (1-self.syst)*gexample.GetMean(2))
            
            
            #gexample_sys.SetPoint(1, self.c1.GetUxmax(), gexample.GetMean(2))
                        #gexample_sys.SetPointError(1, 0, self.syst*gexample.GetMean(2))   
            #gexample_sys.Draw("3")
            self.c1.Modified()
            haxis.Draw("AXISSAME")
            self.c1.Modified()
            gall.Draw("P")
            gexample.Draw("P")

            print 'all rms', gall.GetRMS(2)/gall.GetMean(2)
            print 'example rms', gexample.GetRMS(2)/gexample.GetMean(2)

            l = ROOT.TLatex()
            l.SetNDC()
            l.SetTextFont(72)
            l.DrawLatex(0.1922,0.867,"ATLAS")
            
            l2 = ROOT.TLatex()
            l2.SetNDC()
            l2.DrawLatex(0.1922,0.811,"Preliminary")

            leg = ROOT.TLegend(0.4309045,0.6765734,0.9572864,0.9213287, "", "brNDC")
            leg.AddEntry(gall,"TileCal average","P")
            #leg.AddEntry(gall_sys, "detector mean syst. error", "F")
            leg.AddEntry(gexample,"Typical channel","P")
            leg.AddEntry(gexample_sys, "\pm 0.7% channel systematic uncertainty", "L") 
            leg.Draw()

            tpave = ROOT.TPave()
            tpave.SetFillColor(0)
            tpave.SetLineColor(0)            
            tpave.DrawPave(0.1268844,0.9562937,0.9849246,0.9965035, 0, "brNDC")


            self.c1.Print("%s/pen_%s_%0.2f.eps" % (self.dir, self.example, gain))
            self.c1.Print("%s/pen_%s_%0.2f.png" % (self.dir, self.example, gain))
            self.c1.Print("%s/pen_%s_%0.2f.root" % (self.dir,self.example, gain))
            
    
    def ProcessRegion(self, region):
        if 'gain' not in region.GetHash():
            return

        found = False
        calibrated = False
        default = False

        calib = 0 # calibation constant
        for event in region.GetEvents():
            if event.runType == self.runType:
                #print event.time
                #print event.time, int(event.getTimeSeconds())
                if event.data.has_key('goodRegion') and event.data['goodRegion'] and\
                   event.data.has_key('goodEvent') and event.data['goodEvent']:

                    # unix/runserver clock starts from 1970, ROOT clock starts from 1995
                    # so then 788923149 is 25 years in seconds
                    #print event.getTimeSeconds(), event.getTimeSeconds() -  788923149
                    time = int(event.getTimeSeconds())/self.factor
                    
                    if self.example in region.GetHash():
                        if 'lowgain' in region.GetHash():
                            self.lo_example[time] = [event.data['calibration']]
                        else:
                            self.hi_example[time] = [event.data['calibration']]

                    if 'lowgain' in region.GetHash():
                        if not self.lo.has_key(time):
                            self.lo[time] = []
                        self.lo[time].append(event.data['calibration'])
                    else:
                        if not self.hi.has_key(time):
                            self.hi[time] = []
                        self.hi[time].append(event.data['calibration'])
            
