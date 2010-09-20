from workers.NoiseWorker import *
from src.oscalls import *
import src.MakeCanvas
import time
from array import array

class NoiseVsDB(NoiseWorker):
    '''Compare noise values from runs with those in DB'''

    def __init__(self, parameter='digi',runType='PedSummary',savePlot=True,type='readout'):
        self.initLog()
        self.runType = runType
        self.savePlot = savePlot
        if savePlot:
            self.initNoiseHistFile()
        self.warnThresh  = 0.05
        self.errorThresh = 0.10

        self.partStr=['LBA','LBC','EBA','EBC']
        
        self.parameters = []
        if parameter == 'digi':
            self.type = 'readout'
            self.parameters = ['hfn','ped']
            self.gainStr=['LG','HG'] 
        elif parameter == 'chan':
            self.type = 'readout'
            self.parameters = ['efit_mean','eopt_mean']
            self.gainStr=['LG','HG'] 
        elif parameter == 'cell':
            self.type = 'physical'
            self.gainStr = ['']
            self.parameters = ['cellnoise'+g for g in ['LGLG','LGHG','HGLG','HGHG']]
        else:
            self.type = type
            if type=='readout': self.gainStr=['LG','HG']
            else: self.gainStr=['']
            self.parameters.append(parameter)
        
        self.cellbins =\
                   [['A00','A01','A02','A03','A04','A05','A06','A07','A08','A09',\
                    'BC00','BC01','BC02','BC03','BC04','BC05','BC06','BC07','BC08','D00','D02','D04','D06'],\
                    ['A00','A01','A02','A03','A04','A05','A06','A07','A08','A09',\
                    'BC00','BC01','BC02','BC03','BC04','BC05','BC06','BC07','BC08','D02','D04','D06'],\
                    ['A11','A12','A13','A14','A15','BC09','BC10','BC11','BC12','BC13','BC14','D08','D10','D12','E10','E11','E13','E15'],\
                    ['A11','A12','A13','A14','A15','BC09','BC10','BC11','BC12','BC13','BC14','D08','D10','D12','E10','E11','E13','E15']]
        


    def ProcessStart(self):
        self.time = time.mktime(time.localtime())
       
        # Mostly just declaring histograms depending on desired level of detail
        if self.savePlot:
            self.noiseHistFile.cd()
            self.dbTree = ROOT.TTree( 'dbTree', 'Tree with DBValues' )
            self.h_diffWarn  = {}
            self.h_diffError = {}
            self.h_runDiffWarn  = {}
            self.h_runDiffError = {}
            self.countWarn  = {}
            self.countError = {}
            self.dbArray = {}
            for p in self.parameters:
                self.h_diffWarn[p] = []
                self.h_diffError[p] = []
                self.h_runDiffWarn[p] = []
                self.h_runDiffError[p] = []
                self.dbArray[p] = []
                self.countWarn[p] = 0
                self.countError[p] = 0
                for part in xrange(4):
                    ncells = len(self.cellbins[part])
                    if self.type =='readout':
                        arMax =64*48
                        self.dbArray[p].append(array( 'f', arMax*[ 0. ] ))
                        self.dbTree.Branch(p+'_'+self.partStr[part], self.dbArray[p][part], p+'_'+self.partStr[part]+'[%i]/F' % arMax)
                    elif self.type == 'physical':
                        arMax = 64*ncells
                        self.dbArray[p].append(array( 'f', arMax*[ 0. ] ))
                        brStr = p+'_'+self.partStr[part]+'[%i]/F' % arMax
                        self.dbTree.Branch(p+'_'+self.partStr[part], self.dbArray[p][part],brStr)
                    self.h_diffWarn[p].append([])
                    self.h_diffError[p].append([])
                    self.h_runDiffWarn[p].append([])
                    self.h_runDiffError[p].append([])
                    for g in xrange(len(self.gainStr)):
                        self.h_diffWarn[p][part].append(ROOT.TH2F('h_'+p+'_diffWarn_'+self.gainStr[g]+'_'+self.partStr[part],'Difference from DB for '+p+' '+self.gainStr[g]+' '+self.partStr[part],64,0,64,48,0,48))
                        self.h_diffError[p][part].append(ROOT.TH2F('h_'+p+'_diffError_'+self.gainStr[g]+'_'+self.partStr[part],'Difference from DB for '+p+' '+self.gainStr[g]+' '+self.partStr[part],64,0,64,48,0,48))
                        self.h_diffWarn[p][part][g].SetXTitle('Module')
                        if self.type == 'readout':
                            self.h_diffWarn[p][part][g].SetYTitle('Channel')
                        elif self.type == 'physical':
                            self.h_diffWarn[p][part][g].SetYTitle('Cell')
                            self.h_diffWarn[p][part][g].GetYaxis().Set(ncells,0,ncells)
                            for b in xrange(len(self.cellbins[part])):
                                self.h_diffWarn[p][part][g].GetYaxis().SetBinLabel(b+1,self.cellbins[part][b])
                        self.h_diffError[p][part][g].SetXTitle('Module')
                        if self.type == 'readout':
                            self.h_diffError[p][part][g].SetYTitle('Channel')
                        elif self.type == 'physical':
                            self.h_diffError[p][part][g].SetYTitle('Cell')
                            self.h_diffError[p][part][g].GetYaxis().Set(ncells,0,ncells)
                            for b in xrange(len(self.cellbins[part])):
                                self.h_diffError[p][part][g].GetYaxis().SetBinLabel(b+1,self.cellbins[part][b])
                        self.h_runDiffWarn[p][part].append(ROOT.TH2F('h_'+p+'_runDiffWarn_'+self.gainStr[g]+'_'+self.partStr[part],'Difference from DB (per run) for '+p+' '+self.gainStr[g]+' '+self.partStr[part],64,0,64,48,0,48))
                        self.h_runDiffError[p][part].append(ROOT.TH2F('h_'+p+'_runDiffError_'+self.gainStr[g]+'_'+self.partStr[part],'Difference from DB (per run) for '+p+' '+self.gainStr[g]+' '+self.partStr[part],64,0,64,48,0,48))
                        self.h_runDiffWarn[p][part][g].SetXTitle('Module')
                        if self.type == 'readout':
                            self.h_runDiffWarn[p][part][g].SetYTitle('Channel')
                        elif self.type == 'physical':
                            self.h_runDiffWarn[p][part][g].SetYTitle('Cell')
                            self.h_runDiffWarn[p][part][g].GetYaxis().Set(len(self.cellbins[part]),0,len(self.cellbins[part]))
                            for b in xrange(len(self.cellbins[part])):
                                self.h_runDiffWarn[p][part][g].GetYaxis().SetBinLabel(b+1,self.cellbins[part][b])
                        self.h_runDiffError[p][part][g].SetXTitle('Module')
                        if self.type == 'readout':
                            self.h_runDiffError[p][part][g].SetYTitle('Channel')
                        elif self.type == 'physical':
                            self.h_runDiffError[p][part][g].SetYTitle('Cell')
                            self.h_runDiffError[p][part][g].GetYaxis().Set(len(self.cellbins[part]),0,len(self.cellbins[part]))
                            for b in xrange(len(self.cellbins[part])):
                                self.h_runDiffError[p][part][g].GetYaxis().SetBinLabel(b+1,self.cellbins[part][b])


    def ProcessStop(self):
        if self.savePlot:
            self.noiseHistFile.cd()
            ROOT.gDirectory.cd('/')
            ROOT.gDirectory.mkdir('Consistency').cd()
            self.dbTree.Fill()
            self.dbTree.Write()
            for p in self.parameters:
                for part in xrange(4):
                    for h in self.h_diffWarn[p][part]:
                        h.Write()
                    for h in self.h_diffError[p][part]:
                        h.Write()
                    for h in self.h_runDiffWarn[p][part]:
                        h.Write()
                    for h in self.h_runDiffError[p][part]:
                        h.Write()
                
                self.noiseLog.warn('Number of update warnings for %s: %i' % (p,self.countWarn[p]) )
                self.noiseLog.error('Number of update errors for %s: %i' % (p,self.countError[p]) )
            
    def ProcessRegion(self, region):
        useRegion = False
        if self.type=='readout' and 'gain' in region.GetHash():
            [part, mod, ch, gain] = region.GetNumber()
        elif self.type=='physical' and '_t' in region.GetHash():
            [part, mod, sample, tower] = region.GetNumber()
            sampStr =['A','BC','D','E']
            ch = self.cellbins[part-1].index(sampStr[sample]+'%02d' % tower)
            ncells = len(self.cellbins[part-1])
            gain = 0
        else:
            return
            
        dbUpdate = {}
        data_sub = {}
        runList = {}
        for event in region.GetEvents():
            if event.runType == self.runType:
                for p in self.parameters:
                    # Retrieve DB values for each run
                    data = event.data[p+'_db']
                    dbUpdate[p] = 0
                    if len(data)==0:
                        continue
                    i = 0
                    # comparte to DB as far back as last update
                    data.reverse()
                    dbVal = data[0]
                    for d in data:
                        i += 1
                        if d!=dbVal:
                            break
                    # store list of real data and runs for which DB was constant
                    data_sub[p]=event.data[p][-(i+1):]
                    runList[p]=event.runNumber[p][-(i+1):]

                    # compare list with data
                    i=0
                    for d in data_sub[p]:
                        if dbVal>10-4:  diff = (1.0*d-dbVal)/dbVal
                        else: diff = 1000.
                        if abs(diff) > self.errorThresh:
                            self.noiseLog.error('%i: %s has %s inconsistent with DB by %.2f%%' % (runList[p][i],region.GetHash(),p,diff*100) )
                            self.h_runDiffError[p][part-1][gain].Fill(mod,ch,1)
                        elif abs(diff) > self.warnThresh:
                            self.noiseLog.warn('%i: %s has %s inconsistent with DB by %.2f%%' % (runList[p][i],region.GetHash(),p,diff*100) )
                            self.h_runDiffWarn[p][part-1][gain].Fill(mod,ch,1)
                        i += 1

                    median = getMedian(data_sub[p])
                    mean   = getMean(data_sub[p])
                    dbVal  = event.data[p+'_db'][-1]
                    # Save db values to an array for later access
                    if self.type == 'readout':
                        self.dbArray[p][part-1][(mod-1)*48+ch]=dbVal
                    elif self.type == 'physical':
                        self.dbArray[p][part-1][(mod-1)*ncells+ch]=dbVal
                    if dbVal>10e-4: diff   = (median-dbVal)/dbVal
                    else: diff = 1000.
                    
                    if abs(diff) > self.errorThresh:
                        self.noiseLog.error('%s --- %s ---- differs from DB by %.2f%%' % (region.GetHash(),p,diff*100) )
                        self.h_diffError[p][part-1][gain].Fill(mod,ch,1)
                        dbUpdate[p] = median
                        self.countError[p] += 1
                    elif abs(diff) > self.warnThresh:
                        self.noiseLog.warn('%s --- %s ---- differs from DB by %.2f%%' % (region.GetHash(),p,diff*100) )
                        self.h_diffWarn[p][part-1][gain].Fill(mod,ch,1)
                        self.countWarn[p] += 1

        region.AddEvent(Event('PedUpdate',-1,dbUpdate,0))
