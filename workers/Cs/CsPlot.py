# Author: Mikhail Makouski <Mikhail.Makouski@cern.ch>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.region import *
import datetime,time
import ROOT
from src.cesium import csdb

class CsPlot(GenericWorker):
    "This worker plots mean value of Cs constants versus time"

    def __init__(self):
        self.c1=ROOT.TCanvas()
        self.c2=ROOT.TCanvas()
        self.c2.Divide(1,2)
        self.grHV=ROOT.TGraph()
        self.grtemp=ROOT.TGraph()
        self.graph=ROOT.TGraphErrors()
        #self.fff=ROOT.TFile("histograms.root","RECREATE")
        
        self.graph.SetMarkerStyle(20)
        self.par=None
        self.sum={}

    def DrawPicture(self):
        print 'ProcessStop'
        time1=datetime.datetime.now()
        p=0
        gp=0
        for t in self.sum:
            #(d,s)=((t-time1).days,(t-time1).seconds)
            #self.graph.SetPoint(p,d+float(s)/86400,self.sum[t]['sum']/self.sum[t]['N'])
           
                    
                
            if self.sum[t]['sum']>0:
                for tt in self.sum:
                    if abs((t-tt).days)<2 and t!=tt and self.sum[tt]['sum']>0:
                        self.sum[t]['hist'].Add(self.sum[tt]['hist'])
                        self.sum[tt]['sum']*=-1
                self.graph.SetPoint(gp,time.mktime(t.timetuple()),self.sum[t]['hist'].GetMean())
                self.graph.SetPointError(gp,0,self.sum[t]['hist'].GetRMS())
                print 'histogram had entries: ',self.sum[t]['hist'].GetEntries()
                print 'mean ',self.sum[t]['hist'].GetMean()
                #ccc=ROOT.TCanvas()
                #self.sum[t]['hist'].Draw()
                #ccc.Print('test.pdf')
                #self.sum[t]['hist'].Write()
                gp+=1
            self.grHV.SetPoint(p,time.mktime(t.timetuple()),self.sum[t]['HV']/self.sum[t]['N'])
            self.grtemp.SetPoint(p,time.mktime(t.timetuple()),self.sum[t]['temp']/self.sum[t]['N'])
            p+=1

        print self.sum
        self.c1.cd()
        self.graph.SetTitle('mean Cs integral values for '+self.par)
        self.graph.Draw('AP')
        
        self.graph.GetXaxis().SetTimeDisplay(1)
        self.graph.GetXaxis().SetTimeOffset(0,'gmt')
        self.graph.GetXaxis().SetTimeFormat('%b-%y')
        #self.graph.GetXaxis().Set
        #------self.graph.GetYaxis().SetRangeUser(1200,3300)

        self.c2.cd(1)
        self.grHV.SetTitle('mean HV for '+self.par)
        self.grHV.Draw('AP')
        self.grHV.GetXaxis().SetTimeDisplay(1)
        self.grHV.GetXaxis().SetTimeOffset(0,'gmt')
        self.grHV.GetXaxis().SetTimeFormat('%b-%y')
        #self.graph.GetXaxis().Set
        self.grHV.GetYaxis().SetRangeUser(600,700)
        
        self.c2.cd(2)
        self.grtemp.SetTitle('temperature of PMTblock '+self.par)
        self.grtemp.Draw('AP')
        self.grtemp.GetXaxis().SetTimeDisplay(1)
        self.grtemp.GetXaxis().SetTimeOffset(0,'gmt')
        self.grtemp.GetXaxis().SetTimeFormat('%b-%y')
        #self.graph.GetXaxis().Set
        self.grtemp.GetYaxis().SetRangeUser(22,25)

        if self.par!=None:
            self.c1.Print('plots/Cs_%s.pdf'%self.par)
            self.c2.Print('plots/HVtemp_%s.pdf'%self.par)
        #self.fff.Write()
        #self.fff.Close()
        
    def ProcessRegion(self,region):
        # We only want channels, not gains/modules/partitions
        #print region.GetHash()
        if '_c' not in region.GetHash() or 'gain' in region.GetHash():
            return

        for event in region.GetEvents():
            if event.runType != 'cesium':
                continue
            if 'csconst' not in event.data:
                continue
                        
            #correct this later
            hash=region.GetHash()
            split=hash.split('_')[1:]
            if self.par==None:
                self.par=split[0]
            if len(split) >= 2:
                mod=int(split[1][1:])
            if len(split) >= 3:
                pmt=csdb.chan2hole(self.par,int(split[2][1:]))

            
            if self.par!='ALL' and self.par!=split[0]:
                print 'You are trying to make average over more than one partitionn!'
                self.par='ALL'
            if event.data['time'] not in self.sum:
                self.sum[event.data['time']]={}#sum,N
                self.sum[event.data['time']]['sum']=0.0
                self.sum[event.data['time']]['temp']=0.0
                self.sum[event.data['time']]['HV']=0.0
                self.sum[event.data['time']]['N']=0
                self.sum[event.data['time']]['hist']=ROOT.TH1F(split[0]+str(mod)+event.data['time'].strftime("%H:%M:%S"),split[0]+str(mod),400,1500,2400)
            cell=csdb.pmt2cell(self.par,pmt)
            print self.par,mod,pmt,cell
            if cell[0]=='E':
                continue
            if event.data['HV']<0:
                continue
            self.sum[event.data['time']]['HV']+=event.data['HV']
            self.sum[event.data['time']]['temp']+=event.data['temp']
            if cell[0]=='D':
                self.sum[event.data['time']]['sum']+=event.data['csconst']/1.2
                self.sum[event.data['time']]['hist'].Fill(event.data['csconst']/1.2)
            else:
                self.sum[event.data['time']]['sum']+=event.data['csconst']
                self.sum[event.data['time']]['hist'].Fill(event.data['csconst'])
            self.sum[event.data['time']]['N']+=1
            
            
