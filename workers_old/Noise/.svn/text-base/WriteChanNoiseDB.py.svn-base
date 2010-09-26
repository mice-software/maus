# WriteChanNoiseDB.py
# Class for writing channel noise to DB
# Author: Brian Martin (brian.thomas.martin@cern.ch)

from src.ReadGenericCalibration import *
from src.region import *
import os

# For reading from DB
from TileCalibBlobPython import TileCalibTools, TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

# For turning off annoying logging
import logging
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger

class WriteChanNoiseDB(ReadGenericCalibration):
    '''Write out channel noise constants to sqlite file'''

    def __init__(self, runType = 'PedUpdate',offline_iov = 'use latest run'):
        self.runType = runType
       
        # Match variable read from root file to DB key
        self.DBdict = { 'efit':'efit_mean','of1':'eopt_mean' }
       
        self.offline_folder = {}
        self.offline_tag = {}
        self.online_folder = {}
        self.online_tag = {}
        self.offline_tag['efit']    = 'TileOfl01NoiseFit-HLT-UPD1-01'
        self.offline_folder['efit'] = TileCalibTools.getTilePrefix()+'NOISE/FIT'
        self.online_tag['efit']     = ""
        self.online_folder['efit']  = TileCalibTools.getTilePrefix(ofl=False)+'NOISE/FIT'
        self.offline_tag['of1']    = 'TileOfl01NoiseOf1-HLT-UPD1-01'
        self.offline_folder['of1'] = TileCalibTools.getTilePrefix()+'NOISE/OF1'
        self.online_tag['of1']     = ""
        self.online_folder['of1']  = TileCalibTools.getTilePrefix(ofl=False)+'NOISE/OF1'
        self.offline_tag['of2']    = 'TileOfl01NoiseOf2-HLT-UPD1-01'
        self.offline_folder['of2'] = TileCalibTools.getTilePrefix()+'NOISE/OF2'
        self.online_tag['of2']     = ""
        self.online_folder['of2']  = TileCalibTools.getTilePrefix(ofl=False)+'NOISE/OF2'
        
        if offline_iov == 'use latest run':
            self.offline_iov_use_latest = True
            self.offline_iov = (MAXRUN,MAXLBK)
        else:
            self.offline_iov_use_latest = False
            self.offline_iov = offline_iov  # The format of this is (run number, lumi block). Example: (92929, 0)
        
        self.online_iov = (0, 0)

    
    def ProcessStart(self):
        PyCintex.makeClass('std::vector<float>')
        
        # open DB connection to sqlite file
        self.db = TileCalibTools.openDb('SQLITE', 'COMP200', 'UPDATE')
        
        loGainDefVec = PyCintex.gbl.std.vector('float')()
        loGainDefVec.push_back(0.0)
        hiGainDefVec = PyCintex.gbl.std.vector('float')()
        hiGainDefVec.push_back(0.0)
        self.defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        self.defVec.push_back(loGainDefVec)
        self.defVec.push_back(hiGainDefVec)
        
        self.blobWriterOnline = {}
        self.blobWriterOffline = {}        
        for key in self.offline_tag.keys():
            self.blobWriterOnline[key]  = TileCalibTools.TileBlobWriter(self.db,self.online_folder[key], 'Flt', False)
            self.blobWriterOffline[key] = TileCalibTools.TileBlobWriter(self.db,self.offline_folder[key], 'Flt', True)
            
        
        
    def ProcessStop(self):
        # iov until is the end of the interval of validity, so infinity here
        iovUntil = (MAXRUN,MAXLBK)
        author   = "%s through Tucs" % os.getlogin()
        
        for key in self.offline_tag.keys():
            #self.blobWriterOnline[key].setComment(author, " ".join(sys.argv))
            #self.blobWriterOnline[key].register(self.online_iov, iovUntil, self.online_tag[key])
    
            self.blobWriterOffline[key].setComment(author, " ".join(sys.argv))
            self.blobWriterOffline[key].register(self.offline_iov, iovUntil, self.offline_tag[key])
            
        self.db.closeDatabase()
            
    def ProcessRegion(self, region):
        if 'gain' not in region.GetHash():
            return

        [part, mod, chan, gain] = region.GetNumber()

        value = None
        for event in region.GetEvents():
            if event.runType == self.runType:
                for key in self.DBdict.keys():
                    try:
                        value = event.data[self.DBdict[key]]
                    except:
                        continue

                    if value:
                        blobObjVersion = 1
                        modBlob = self.blobWriterOnline[key].getDrawer(part, mod-1)
                        if modBlob:
                            modBlob.init(self.defVec, 48,blobObjVersion)
                            modBlob = self.blobWriterOnline[key].getDrawer(part, mod-1)
                            modBlob.setData(chan, gain, 0, float(value))

                        modBlob = self.blobWriterOffline[key].getDrawer(part, mod-1)
                        if modBlob:
                            modBlob.init(self.defVec, 48,blobObjVersion)
                            modBlob = self.blobWriterOffline[key].getDrawer(part, mod-1)
                            modBlob.setData(chan, gain,  0, float(value))
        
                
