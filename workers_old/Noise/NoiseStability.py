from workers.NoiseWorker import *
from src.oscalls import *
import src.MakeCanvas
import time
    
class NoiseStability(NoiseWorker):
    "Monitor stability of noise constants"

    def __init__(self, parameter='digi',runType='Ped',savePlot=True,plotDetail=3):
        self.initLog()
        self.runType = runType
        self.savePlot = savePlot
        if savePlot:
            self.initNoiseHistFile()
        self.plotDetail = plotDetail
        self.warnThresh  = 0.05
        self.errorThresh = 0.10

        self.partStr=['LBA','LBC','EBA','EBC']
        
        self.parameters = []
        if parameter == 'digi':
            self.type = 'readout'
            self.gainStr=['LG','HG'] 
            self.parameters = ['hfn','ped']
        elif parameter == 'chan':
            self.type = 'readout'
            self.gainStr=['LG','HG'] 
            self.parameters = ['efit_mean','eopt_mean']
        elif parameter == 'cell':
            self.type = 'physical'
            self.gainStr = [""]
            self.parameters = ['cellnoise'+g for g in ['LGLG','LGHG','HGLG','HGHG']]
        else:
            self.type = type
            if type=='readout': self.gainStr=['LG','HG']
            else: self.gainStr=[""]
            self.parameters.append(parameter)

        self.cellbins =\
                   [['A00','A01','A02','A03','A04','A05','A06','A07','A08','A09','A11','A12','A13','A14','A15',\
                    'BC00','BC01','BC02','BC03','BC04','BC05','BC06','BC07','BC08','D00','D02','D04','D06'],\
                    ['A00','A01','A02','A03','A04','A05','A06','A07','A08','A09','A11','A12','A13','A14','A15',\
                    'BC00','BC01','BC02','BC03','BC04','BC05','BC06','BC07','BC08','D02','D04','D06'],\
                    ['A11','A12','A13','A14','A15','BC09','BC10','BC11','BC12','BC13','BC14','D08','D10','D12','E10','E11','E13','E15'],\
                    ['A11','A12','A13','A14','A15','BC09','BC10','BC11','BC12','BC13','BC14','D08','D10','D12','E10','E11','E13','E15']]


    def ProcessStart(self):
        self.time = time.mktime(time.localtime())
        if self.savePlot:
            self.h_rmsOverMean = {}
            self.h_rmsOverMedian = {}
            if self.plotDetail > 0:
                self.h_timelinePart = {}
                if self.plotDetail > 1:
                    self.h_timelineMod = {}
                    if self.plotDetail > 2:
                        self.h_timelineCh   = {}
                        self.h_timelineCell = {}
            for p in self.parameters:
                self.h_rmsOverMean[p] = []
                self.h_rmsOverMedian[p] = []
                if self.plotDetail >0:
                    self.h_timelinePart[p] = []
                    if self.plotDetail > 1:
                        self.h_timelineMod[p] = []
                        if self.plotDetail > 2:
                            self.h_timelineCh[p]    = []
                            self.h_timelineCell[p] = []
                for part in xrange(4):
                    self.h_rmsOverMean[p].append([])
                    self.h_rmsOverMedian[p].append([])
                    ncells=len(self.cellbins[part])
                    for g in xrange(len(self.gainStr)):
                        self.h_rmsOverMean[p][part].append(ROOT.TH2F('h_'+p+'_rmsOverMean_'+self.gainStr[g]+'_'+self.partStr[part],'RMS over mean for '+p+' '+self.gainStr[g]+' '+self.partStr[part],64,0,64,48,0,48))
                        self.h_rmsOverMean[p][part][g].SetXTitle('Module')
                        if self.type == 'readout':
                            self.h_rmsOverMean[p][part][g].SetYTitle('Channel')
                        elif self.type == 'physical':
                            self.h_rmsOverMean[p][part][g].SetYTitle('Cell')
                            self.h_rmsOverMean[p][part][g].GetYaxis().Set(ncells,0,ncells-1)
                        self.h_rmsOverMedian[p][part].append(ROOT.TH2F('h_'+p+'_rmsOverMedian_'+self.gainStr[g]+'_'+self.partStr[part],'RMS over median for '+p+' '+self.gainStr[g]+' '+self.partStr[part],64,0,64,48,0,48))
                        self.h_rmsOverMedian[p][part][g].SetXTitle('Module')
                        if self.type == 'readout':
                            self.h_rmsOverMedian[p][part][g].SetYTitle('Channel')
                        elif self.type == 'physical':
                            self.h_rmsOverMedian[p][part][g].SetYTitle('Cell')
                            self.h_rmsOverMedian[p][part][g].GetYaxis().Set(ncells,0,ncells-1)
                    if self.plotDetail >0:
                        self.h_timelinePart[p].append([])
                        for g in xrange(len(self.gainStr)):
                            self.h_timelinePart[p][part].append(ROOT.TGraph())
                            self.h_timelinePart[p][part][g].SetNameTitle('h_'+p+'_timeline_'+self.gainStr[g]+'_'+self.partStr[part],'Timeline of '+p+' '+self.gainStr[g]+' '+self.partStr[part])
                        if self.plotDetail >1:
                            for mod in xrange(64):
                                self.h_timelineMod[p].append([])
                                for g in xrange(len(self.gainStr)):
                                    self.h_timelineMod[p][part*64+mod].append(ROOT.TGraph())
                                    self.h_timelineMod[p][part*64+mod][g].SetNameTitle('h_'+p+'_timeline_'+self.gainStr[g]+'_'+self.partStr[part]+str(mod+1).zfill(2),'Timeline of '+p+' '+self.gainStr[g]+' '+self.partStr[part]+str(mod+1).zfill(2))
                                if self.plotDetail >2:
                                    if self.type=='readout':
                                        for ch in xrange(48):
                                            self.h_timelineCh[p].append([])
                                            for g in xrange(len(self.gainStr)):
                                                self.h_timelineCh[p][part*64*48+mod*48+ch].append(ROOT.TGraph())
                                                self.h_timelineCh[p][part*64*48+mod*48+ch][g].SetNameTitle('h_'+p+'_timeline_'+self.gainStr[g]+'_'+self.partStr[part]+str(mod+1).zfill(2)+'_ch'+str(ch).zfill(2),'Timeline of '+p+' '+self.gainStr[g]+' '+self.partStr[part]+str(mod+1).zfill(2)+' ch'+str(ch).zfill(2))
                                    elif self.type=='physical':
                                        for ch in xrange(ncells):
                                            self.h_timelineCell[p].append([])
                                            for g in xrange(len(self.gainStr)):
                                                self.h_timelineCell[p][part*64*ncells+mod*ncells+ch].append(ROOT.TGraph())
                                                self.h_timelineCell[p][part*64*ncells+mod*ncells+ch][g].SetNameTitle('h_'+p+'_timeline_'+self.gainStr[g]+'_'+self.partStr[part]+str(mod+1).zfill(2)+'_cell'+self.cellbins[part][ch],'Timeline of '+p+' '+self.gainStr[g]+' '+self.partStr[part]+str(mod+1).zfill(2)+' cell '+self.cellbins[part][ch])

    def ProcessStop(self):
        if self.savePlot:
            self.noiseHistFile.cd()
            ROOT.gDirectory.mkdir('Stability').cd()
            for p in self.parameters:
                for part in xrange(4):
                    for h in self.h_rmsOverMean[p][part]:
                        h.Write()
                    for h in self.h_rmsOverMedian[p][part]:
                        h.Write()
                    if self.plotDetail>0:
                        for h in self.h_timelinePart[p][part]:
                            h.Write()
                        if self.plotDetail>1:
                            if not ROOT.gDirectory.GetDirectory(self.partStr[part]):
                                ROOT.gDirectory.mkdir(self.partStr[part])
                            ROOT.gDirectory.cd(self.partStr[part])
                            for mod in xrange(64):
                                for h in self.h_timelineMod[p][part*64+mod]:
                                    h.Write()
                                if self.plotDetail>2 and self.type=='readout':
                                    modDir = self.partStr[part]+str(mod+1).zfill(2)
                                    if not ROOT.gDirectory.GetDirectory(modDir):
                                        ROOT.gDirectory.mkdir(modDir)
                                    ROOT.gDirectory.cd(modDir)
                                    for ch in xrange(48):
                                        for h in self.h_timelineCh[p][part*64*48+mod*48+ch]:
                                            h.Write()
                                    ROOT.gDirectory.cd('../')
                            ROOT.gDirectory.cd('../')
    
    def ProcessRegion(self, region):
        useRegion = False
        if self.type=='readout' and 'gain' in region.GetHash():
            [part, mod, ch, gain] = region.GetNumber()
        elif self.type=='physical' and '_t' in region.GetHash():
            [part, mod, sample, tower] = region.GetNumber()
            sampStr =['A','BC','D','E']
            ncells = len(self.cellbins[part-1])
            ch = self.cellbins[part-1].index(sampStr[sample]+'%02d' % tower)
        else:
            return
        data    = {}
        nPoints = {}
        runList = {}
        for p in self.parameters:
            if self.type=='physical':
                gain = 0
            data[p]    = []
            data[p+'_db'] = []
            nPoints[p] = 0
            runList[p] = []
        
        for event in region.GetEvents():
            if event.runType == self.runType:
                useRegion=True
                for p in self.parameters:
                    if event.data[p]!=0.: #Filter out unwanted data
                        data[p].append(event.data[p])
                        data[p+'_db'].append(event.data[p+'_db'])
                        if self.savePlot:
                            if self.plotDetail >0:
                                self.h_timelinePart[p][part-1][gain].SetPoint(nPoints[p],event.runNumber,event.data[p])
                                if self.plotDetail >1:
                                    self.h_timelineMod[p][(part-1)*64+mod-1][gain].SetPoint(nPoints[p],event.runNumber,event.data[p])
                                    if self.plotDetail >2:
                                        if self.type=='readout':
                                            self.h_timelineCh[p][(part-1)*64*48+(mod-1)*48+ch][gain].SetPoint(nPoints[p],event.runNumber,event.data[p])
                                        elif self.type=='physical':
                                            self.h_timelineCell[p][(part-1)*64*ncells+(mod-1)*ncells+ch][gain].SetPoint(nPoints[p],event.runNumber,event.data[p])
                        nPoints[p] += 1 #increment nPoints that have usable data
                        runList[p].append(event.runNumber)
        
        region.AddEvent(Event('PedSummary',runList,data,0))
        
        # don't go any further if no usable data for region
        if not useRegion:
            return

        for p in self.parameters:
            mean   = getMean(data[p])
            rms    = getRMS(data[p])
            median = getMedian(data[p])

            if mean > 0.: variationMean = rms/mean
            else: variationMean = 1.0
            if median > 0.: variationMedian = rms/median
            else: variationMedian = 1.0

            #if variationMean > self.errorThresh:
            #    self.noiseLog.error('%s has unstable %s with variation of %f%%' % (region.GetHash(),p,variationMean*100) )
            #elif variationMean > self.warnThresh:
            #    self.noiseLog.warn('%s has unstable %s with variation of %f%%' % (region.GetHash(),p,variationMean*100) )

            if self.savePlot:
                if variationMean>1.0:
                    variationMean=1.0
                if variationMedian>1.0:
                    variationMedian=1.0
                if variationMean > 1.0:
                    self.h_rmsOverMean[p][part-1][gain].Fill(mod-1,ch,1.0)
                else:
                    self.h_rmsOverMean[p][part-1][gain].Fill(mod-1,ch,variationMean)
                if variationMedian > 1.0:
                    self.h_rmsOverMedian[p][part-1][gain].Fill(mod-1,ch,1.0)
                else:
                    self.h_rmsOverMedian[p][part-1][gain].Fill(mod-1,ch,variationMedian)

