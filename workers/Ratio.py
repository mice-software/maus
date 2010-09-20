# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

import ROOT
from src.GenericWorker import *
from src.MakeCanvas import *

class Ratio(GenericWorker):
    "Make Ratio of constants"

    def __init__(self, runType1 = 'cesium', constant1 = 'csconst', runType2 = 'Laser', constant2 = 'calib_const'):
        self.runType1 =  runType1
        self.constant1 = constant1
        self.runType2 =  runType2
        self.constant2 = constant2
        
    def ProcessRegion(self, region):
        data1 = {}
        data2 = {}

        if len(region.GetEvents()) == 0:
            return

        allevents = set()
        allevents = allevents | region.GetEvents()

        if region.GetChildren():
            for child in region.GetChildren():
                allevents = allevents | child.events

        for event in allevents:
            if event.runType == self.runType1:
                if event.data.has_key(self.constant1):
                    data1[event.runNumber] = event.data[self.constant1]
            if event.runType == self.runType2:
                if event.data.has_key(self.constant2):
                    data2[event.runNumber] = event.data[self.constant2]
                    
        c1 = MakeCanvas()
        g = ROOT.TGraph()
        g.SetMarkerStyle(20)
        i = 0
        for key in data1.keys():
            if data2.has_key(key):
                print 'hooray!!', data1[key], data2[key]
                g.SetPoint(i, key, data1[key]/data2[key])
                i = i + 1
            
        if i != 0:
            g.Draw('AP')
            c1.Print('plots/ratio_%s.pdf' % region.GetHash())
        
        


