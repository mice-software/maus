# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# For DB constants
#
# March 04, 2009
#
# Adapted by Mikhail Makouski <makouski@mail.cern.ch>
# June 2009

from src.ReadGenericCalibration import *
from src.region import *
from src.cesium.chanlists import deadchan,wrongchan
import os

# For reading from DB
from TileCalibBlobPython import TileCalibTools, TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

# For turning off annoying logging
import logging
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger

class WriteCsDB(ReadGenericCalibration):
    "write out a tileSqlite.db file with database constants"

    def __init__(self, offline_tag = 'HLT-UPD1-01',\
                 offline_iov = 'use latest run',register=False, def_iov=75815,threshold=0.05):
        self.runType = 'cesium'
        self.register=register
        # iov to get default constants
        self.defrun=def_iov
        # maximum deviation of default that not make warning
        self.thre=threshold
        
        if self.runType == 'cesium':
            offline_tag = 'TileOfl02CalibCes-%s' % offline_tag
        else:
            print 'WriteDB: Failed to initialize'
            return
        
        ##################
        # Offline folder #
        ##################
        if offline_iov == 'use latest run':
            self.offline_iov_use_latest = True
            self.offline_iov = (MAXRUN,MAXLBK)
        else:
            self.offline_iov_use_latest = False
            self.offline_iov = offfline_iov
            # The format of this is (run number, lumi block). Example: (92929, 0)
            
        self.offline_folder = "/TILE/OFL02/CALIB/CES"
        self.offline_tag = offline_tag
    
    def ProcessStart(self):
        logger = getLogger("TileCalibTools")
        logger.setLevel(logging.error)
        PyCintex.makeClass('std::vector<float>')
        
        # open DB connection to sqlite file
        self.db = TileCalibTools.openDb('SQLITE', 'COMP200', 'UPDATE')
        
        # default CES constants
        DefVec = PyCintex.gbl.std.vector('float')()
        #this default values are used as initialization parameters
        DefVec.push_back(0.99) # cesium constant
        DefVec.push_back(0.99) # laser slope
        DefVec.push_back(699.99) # reference HV
        DefVec.push_back(19.99) # temperature
        self.defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        self.defVec.push_back(DefVec)

        self.blobWriterOffline = TileCalibTools.TileBlobWriter(self.db,self.offline_folder, 'Flt', True)
        self.csblobReader=TileCalibTools.TileBlobReader(self.db, self.offline_folder, self.offline_tag)
        
    def ProcessStop(self):
        # iov until is the end of the interval of validity, so infinity here
        iovUntil = (MAXRUN,MAXLBK)
        author   = "Mikhail Makouski through Tucs" #% os.getlogin()

        if self.register:
            print author, " ".join(sys.argv)
            print self.offline_iov, iovUntil, self.offline_tag
        
            self.blobWriterOffline.setComment(author, " ".join(sys.argv))
            self.blobWriterOffline.register(self.offline_iov, iovUntil, self.offline_tag)
            
        self.db.closeDatabase()
            
    def ProcessRegion(self, region):
        num=region.GetNumber(1)
        if len(num)!=2:
            return
        # this region is module
        
        par=region.GetParent().GetName()
        ros=int(num[0])
        mod=int(num[1])
        calibration = False
        iov=None
        for event in region.GetEvents():
            if event.data.has_key('PMT'):
                if event.data['PMT']:
                    calibration=True
                    iov=event.runNumber
                    # this module has some channels calibrated 

        blobObjVersion = 1
        if calibration:
            # prepare this module for writing
            modBlob = self.blobWriterOffline.getDrawer(ros, mod-1)
            if modBlob:
                modBlob.init(self.defVec,48,blobObjVersion)
            csDrawer  = self.csblobReader.getDrawer(ros, mod-1, (self.defrun, 0))
            # write 'defaults' - what is in the DB
            for chan in xrange(0,48):
                for index in xrange(0,4):
                    modBlob.setData(chan, 0, index, csDrawer.getData(chan,0,index))
        else:
            # this module is not calibrated
            # message will be printed only if there was at least one run during
            # requested period
            if region.GetEvents():
                print 'module not calibrated',par,mod
            return
        
        for physchan in region.GetChildren('readout'):
            num=physchan.GetNumber()
            chan=num[2]
            num=physchan.GetNumber(1)
            pmt=num[2]

            cesium=0
            HV = -0.1
            temp = 0.1
            runN=0
            calibration=False
            for event in physchan.GetEvents():
                if event.runType == self.runType:
                    if event.data.has_key('calibration'):
                        if event.data['csRun']>runN:
                            if event.data['integrator']:
                                calibration = event.data['calibration']/event.data['integrator']
                            if HV>0:
                                if abs(HV-event.data['HV'])>1.5:
                                    print "HV was changed",par,mod,"pmt",pmt
                            HV=event.data['HV']
                            cesium=event.data['calibration']
                            runN=event.data['csRun']
                            temp=self.FindTemp(region,runN)
                            
                    if self.offline_iov_use_latest and self.offline_iov[0] > event.runNumber:
                        self.offline_iov = (event.runNumber, 0)

           
                            
            if calibration:
                if par in wrongchan and mod in wrongchan[par] and pmt in wrongchan[par][mod]:
                    # this channel is bad in Cs but this is the way it works
                    print 'wrongchan',par,mod,pmt,'calibration',calibration,\
                          'default',csDrawer.getData(chan,0,0)
                else:
                    if abs(cesium-csDrawer.getData(chan,0,0))>self.thre:
                        print 'more than',self.thre,'from default ',par,"%02i"%mod,"%02i"%pmt,\
                              "run",runN,calibration,'def',csDrawer.getData(chan,0,0)
                        continue
                #print "writing ",par,mod,chan,calibration,HV,temp
                modBlob.setData(chan, 0, 0, float(calibration))
                modBlob.setData(chan, 0, 2, float(HV))
                modBlob.setData(chan, 0, 3, float(temp))
            else:
                print 'channel not calibrated', par,mod,pmt
                pass

            
    def FindTemp(self,module,csrun):
        temp=19.99
        for evt in module.GetEvents():
            if evt.data.has_key('csRun'):
                if csrun==evt.data['csRun']:
                    temp=evt.data['temp']
        return temp
