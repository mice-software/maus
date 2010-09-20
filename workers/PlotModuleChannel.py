# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas

class PlotModuleChannel(GenericWorker):
    "Make a plot of module number versus channel number for all partitions"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self,\
                 runType1 = 'CIS', parameter1='isCalibrated', text1='Is Calibrated', invert1 = False,\
                 runType2 = 'CIS', parameter2='isDefault',    text2='Is Default', invert2 = False,):
        self.runType1 = runType1
        self.runType2 = runType2

        self.parameter1 = parameter1
        self.parameter2 = parameter2

        self.text1 = text1
        self.text2 = text2

        self.invert1 = invert1
        self.invert2 = invert2

        self.dir = getPlotDirectory()
        self.fileSuffix =  '_'.join([runType1, parameter1, str(invert1), '_'.join(text1.split(' ')), runType2, parameter2, str(invert2), '_'.join(text2.split(' '))])
            

    def ProcessStart(self):
        self.graphs1 = [ROOT.TGraph() for i in range(4)]
        self.graphs1i = [0 for i in range(4)]
        
        self.graphs2 = [ROOT.TGraph() for i in range(4)]
        self.graphs2i = [0 for i in range(4)]

    def ProcessStop(self):
        self.c1.cd()

        self.c1.Print("%s/mod_chan_%s.ps[" % (self.dir, self.fileSuffix))

        for i in range(4):
            # todo fixme, this should be in src/region.py
            if   i == 0: name = 'LBA'
            elif i == 1: name = 'LBC'
            elif i == 2: name = 'EBA'
            elif i == 3: name = 'EBC'

            graph = self.graphs1[i]
            if graph.GetN():
                graph.Draw("AP")
                graph.SetFillColor(0)
                graph.GetXaxis().SetTitle("Module Number")
                graph.GetYaxis().SetTitle("Channel Number within Module")
                graph.GetXaxis().SetLimits(0.2,65.8)
                graph.GetYaxis().SetRangeUser(0.2,48.8)
                
                graph.SetMarkerStyle(20)
                graph.SetMarkerSize(2.0)
                graph.SetMarkerColor(ROOT.kRed)            
                graph.Draw("AP")

            graph2 = self.graphs2[i]
            if graph2.GetN():
                graph2.Draw("AP")
                graph2.SetFillColor(0)
                graph2.GetXaxis().SetTitle("Module Number")
                graph2.GetYaxis().SetTitle("Channel Number within Module")
                graph2.GetXaxis().SetLimits(0.2,65.8)
                graph2.GetYaxis().SetRangeUser(0.2,48.8)
                                                
                graph2.SetMarkerStyle(20)
                graph2.SetMarkerSize(1.3)
                graph2.SetMarkerColor(ROOT.kBlue)

            if graph.GetN():  graph.Draw("AP")
            if graph2.GetN() and not graph.GetN():  graph2.Draw("AP")
            elif graph2.GetN(): graph2.Draw("P")

            leg = ROOT.TLegend(0.6190,0.96,1.0,1.0)
            leg.SetBorderSize(0)
            leg.SetFillColor(0)

            if graph.GetN():  leg.AddEntry(graph, self.text1,"p")
            if graph2.GetN(): leg.AddEntry(graph2, self.text2,"p")

            leg.Draw()
            
            pt = ROOT.TPaveText(0.1017812,0.96,0.3053435,1.0, "brNDC")
            pt.AddText("%s Status" % name)
            pt.SetBorderSize(0)
            pt.SetFillColor(0)
            pt.Draw()
            
            self.c1.Print("%s/mod_chan_%s.ps" % (self.dir, self.fileSuffix))
            self.c1.Print("%s/mod_chan_%s.root" % (self.dir, self.fileSuffix))
            self.c1.Clear()
                    
        self.c1.Print("%s/mod_chan_%s.ps]" % (self.dir, self.fileSuffix))
                
        
        
    def ProcessRegion(self, region):
        if 'gain' not in region.GetHash():
            return

        x, y, z, w = region.GetNumber()
        x = x - 1
        for event in region.GetEvents():
            if event.runType == self.runType1 and 'low' in region.GetHash():
                if event.data.has_key(self.parameter1):
                    if not self.invert1 and event.data[self.parameter1]:
                        self.graphs1[x].SetPoint(self.graphs1i[x], y, z)
                        self.graphs1i[x] += 1
                        break

                    if self.invert1 and not event.data[self.parameter1]:
                        self.graphs1[x].SetPoint(self.graphs1i[x], y, z)
                        self.graphs1i[x] += 1
                        break

        for event in region.GetEvents():
            if event.runType == self.runType2 and 'high' in region.GetHash():
                if event.data.has_key(self.parameter2):
                    if not self.invert2 and event.data[self.parameter2]:
                        self.graphs2[x].SetPoint(self.graphs2i[x], y, z)
                        self.graphs2i[x] += 1
                        break
                            
                    if self.invert2 and not event.data[self.parameter2]:
                        self.graphs2[x].SetPoint(self.graphs2i[x], y, z)
                        self.graphs2i[x] += 1
                        break
                                                                                                                                                                    

                
                
