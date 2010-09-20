# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import time
import datetime

class TimeStability(GenericWorker):
    "Compute the time stability of a calibration constant"

    def __init__(self, runType = 'CIS', parameter='calibration', savePlot=True, all=False,):
        self.runType = runType
        self.parameter = parameter
        self.savePlot = savePlot
        self.all = all

        self.dir = getPlotDirectory()
        createDir('%s/time_stability' % self.dir)

    def ProcessStart(self):
        ROOT.gStyle.SetOptStat(11111111)
        self.c1 = src.MakeCanvas.MakeCanvas()
        
        self.time = time.mktime(time.localtime())
        self.hlo = ROOT.TH1F('hlotime', '', 20, 0, 1)
        self.hhi = ROOT.TH1F('hhitime', '', 20, 0, 1)
        self.hlo.SetLineColor(ROOT.kBlue)
        self.hhi.SetLineColor(ROOT.kRed)

    def ProcessStop(self):
        self.c1.SetLogy(1)
        self.c1.cd()
        ROOT.gStyle.SetOptStat(11111111)
        self.c1.Modified()
        self.c1.Update()
        self.c1.Modified()
        self.hlo.GetXaxis().SetTitle('RMS')
        self.hhi.GetXaxis().SetTitle('RMS')
        ROOT.gStyle.SetOptStat(11111111)
        self.hlo.SetStats(1)
        if self.hlo.GetEntries():
            self.hlo.Draw('')
            if self.hhi.GetEntries():
                self.hhi.Draw('SAME')
        elif self.hhi.GetEntries():
            self.hhi.Draw()
            
        self.c1.Modified()
        self.c1.Update()
        self.c1.Modified()
        ROOT.gStyle.SetOptStat(11111111)
        self.c1.Modified()
        self.c1.Update()
        self.c1.Modified()
        leg = ROOT.TLegend(0.5100503,0.6066434,0.7148241,0.7447552, "","brNDC")
        leg.SetBorderSize(0)
        leg.SetFillColor(0)
        leg.AddEntry(self.hhi,"high-gain","l")
        leg.AddEntry(self.hlo,"low-gain","l")
        leg.Draw()

        self.c1.Print('%s/time_stability/time_spread_rms.eps' % self.dir)
        self.c1.Print('%s/time_stability/time_spread_rms.root' % self.dir)
        self.c1.SetLogy(0)

    def ProcessRegion(self, region):
        if 'gain' not in region.GetHash():
            return

        gain = -1
        if 'low' in region.GetHash():
            gain = 0
        else:
            gain = 1
            
        found = False
        calibrated = False
        default = False

        calib = 0 # calibation constant
        for event in region.GetEvents():
            if event.data.has_key('calibration_db'):
                calib = event.data['calibration_db']
            if event.runType == self.runType and event.data.has_key('calibratableEvent'):
                if event.data['calibratableEvent']:
                    calibrated = True

                if event.data.has_key('problems') and \
                       event.data['problems'].has_key('Default Calibration') and\
                       event.data['problems']['Default Calibration']:
                    default = True
                    
                found = True
                    
        if (not found or calibrated) and not self.all:
            return

        #print 'howdy'

        graph = ROOT.TGraph() 
        graphi = 0
        rms = -99999
        mean = -99999

        for event in region.GetEvents():
            if event.runType == self.runType:
                if (event.data.has_key('goodEvent') and event.data['goodEvent']) or \
                   (event.data.has_key('isBad') and not event.data['isBad']):
                    graph.SetPoint(graphi, (event.getTimeSeconds() - self.time)/60/60/24, event.data[self.parameter])
                    graphi += 1

        gain_theory = 0.0
        if gain == 0:
            gain_theory = 1.27874994278
        else:
            gain_theory = 81.8399963379
            
        if graphi != 0:
            graph.Draw('AP')
            graph.SetMinimum(gain_theory*0.92)
            graph.SetMaximum(gain_theory*1.08)
            graph.SetMarkerStyle(20)
            graph.SetMarkerSize(1.3)
            dt = datetime.date.today().strftime("%d %B %Y")
            graph.GetXaxis().SetTitle('Days till %s' % dt)
            graph.GetYaxis().SetTitle('CIS Constant (ADC count/pC)')
            graph.SetMarkerColor(ROOT.kBlack)

            hr = self.c1.DrawFrame(-365., gain_theory*0.92, 1, gain_theory*1.08);
            

            ghist = graph.GetHistogram()

            self.c1.Update()
            self.c1.Modified()
            box2 = ROOT.TBox(self.c1.GetUxmin(), gain_theory * 0.98,\
                            self.c1.GetUxmax(), gain_theory * 1.02)
            box2.SetFillColor(1)#ROOT.kSpring-4)
            box2.SetFillStyle(3005)
            box2.Draw()


            box = ROOT.TBox(self.c1.GetUxmin(), calib * 0.99,\
                            self.c1.GetUxmax(), calib * 1.01)
            box.SetFillColor(1)#ROOT.kSpring)
            box.SetFillStyle(3004)
            box.Draw()

            line = ROOT.TLine() # x1 y1 x2 y2
            line.SetLineWidth(3)
            line.DrawLine(self.c1.GetUxmin(), gain_theory, self.c1.GetUxmax(), gain_theory)
            line.DrawLine(self.c1.GetUxmin(), calib, self.c1.GetUxmax(), calib)

            ghist.Draw("AXIS SAME")
            graph.Draw("P") # Draw points over box

            leg = ROOT.TLegend(0.6432161,0.8391608,0.9346734,0.9423077, "","brNDC")
            leg.SetBorderSize(0)
            leg.SetFillColor(0)
            leg.AddEntry(box2,"\pm2% of nominal","f")
            leg.AddEntry(box,"\pm1% of db","f")
            leg.Draw()

            tl = ROOT.TLatex()
            tl.SetTextAlign(12)
            tl.SetTextSize(0.04)
            tl.SetNDC()

            rms = graph.GetRMS(2)
            mean = graph.GetMean(2)

            tl.DrawLatex(0.19, 0.85,"Mean = %0.2f" % mean)
 #           tl.DrawLatex(0.15, 0.80,"RMS = %f" % rms)

#            tl.DrawLatex(0.15, 0.75,"RMS / default calib = %f" % (rms/gain_theory*100))
#            if default:
#                tl.DrawLatex(0.15, 0.70,"Default Calibration in DB")

            cstr = region.GetHash()[8:]
            pstr = region.GetHash(1)[16:19]
            csplit = cstr.split('_')
            newstr = csplit[0] + csplit[1][1:] + ' ' + csplit[2] + '/' + pstr + ' ' + csplit[3]
            
            tl.DrawLatex(0.1670854,0.9685315, newstr)
            
            if 'lowgain' in region.GetHash():
                self.hlo.Fill(rms/gain_theory*100) # 1.27874994278 is default calib
            elif 'highgain' in region.GetHash():
                self.hhi.Fill(rms/gain_theory*100) # 81.8399963379 is default calib
            
            if self.savePlot and (rms/gain_theory*100 > 0.1 or self.all):
                self.c1.Print('%s/time_stability/timestab_%s.pdf' % (self.dir, region.GetHash()))
                #self.c1.Print('%s/time_stability/timestab_%s.root' % (self.dir, region.GetHash()))
                
                

        newevents = set()
        for event in region.GetEvents():
            if event.runType == self.runType:
                if rms != -99999 and mean != -99999:
                    event.data['rms'] = rms
                    event.data['mean'] = mean
                                 
            newevents.add(event)

        region.SetEvents(newevents)

        
            
