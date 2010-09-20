# Author: Mikhail Makouski <Mikhail.Makouski@cern.ch>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.region import *
from src.cesium import csdb
#^my functions to read Cs files
import datetime
#import MySQLdb


class ReadCsFile(GenericWorker):
    "the second approximation to Cs cailbration reader"
    def __init__(self, processingDir='tmp',normalize=True):
        # directory where ROOT ntuples are
        self.processingDir=processingDir
        # keep ADC counts or divide by reference value
        self.normalize=normalize

    def ProcessStart(self):
        pass

    def ProcessStop(self):
        csdb.closeall()
    
    def ProcessRegion(self, region):
        num=region.GetNumber(1)
        if len(num)!=2:
            return
        # this region is module
        par=region.GetParent().GetName()
        mod=int(num[1])
        for evt in region.GetEvents():
            #cycle over events -- runs for this module
            if evt.runType!='cesium': return
            csrun=evt.data['csRun']
            
            csdb.rawpath=self.processingDir+'/'+str(evt.time.year)+'/data'
            csdb.intpath=self.processingDir+'/'+str(evt.time.year)+'/int'
                
            csconsts=csdb.readces(csrun,par,mod)
            hvtemp=csdb.readhv(csrun,par,mod)

            if not hvtemp:
                print 'no hvtemp!'
                hv=-1
                temp=-1
                continue
            
            hv,temp=hvtemp
            norm=1.0
            if self.normalize:
                norm=csdb.intnorm(evt.data['source'],evt.time)
            # we have read calibration constants from files
            # distribute them over 'children'             
            for chan_region in region.GetChildren('readout'):
                chnum=chan_region.GetNumber(1)
                pmt=chnum[2]
                for ch_ev in chan_region.GetEvents():
                    if ch_ev.data['csRun']==evt.data['csRun']:
                        if csconsts[pmt]>3000 or csconsts[pmt]<100:
                            print "bad csconst!",csrun,par,mod,pmt
                        else:
                            ch_ev.data['calibration']=csconsts[pmt]/norm
                        ch_ev.data['HV']=hv[pmt]
                        
            # temperature is measured for a whole module
            evt.data['temp']=temp
                                
