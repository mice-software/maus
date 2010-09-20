# WriteDigiNoiseDB.py
# Class for writing digital noise to DB
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

class WriteDigiNoiseDB(ReadGenericCalibration):
    '''Write out noise constants to sqlite file'''

    def __init__(self, runType = 'PedUpdate', parameter='all',offline_iov = 'use latest run',load_autocr=False):
        self.runType = runType
        self.load_autocr = load_autocr
        self.indexDB = {'ped':0,'hfn':1}
        for x in xrange(6):
            self.indexDB['autocorr'+str(x)] = x

        self.parameters = []
        if parameter =='all':
            self.parameters = ['ped','hfn']
            if load_autocr:
                self.parameters += ['autocorr'+str(x) for x in xrange(6)]
        else:
            self.parameters.append(parameter)
       
        self.offline_folder = {}
        self.offline_tag = {}
        self.online_folder = {}
        self.online_tag = {}
        for key in ['Digi','autocr']:
            if key == 'Digi':
                offline_tag = 'TileOfl01NoiseSample-HLT-UPD1-01'
                folder = 'NOISE/SAMPLE'
            else:
                offline_tag = 'TileOfl01NoiseAutocr-HLT-UPD1-00'
                folder = 'NOISE/AUTOCR'
            self.offline_folder[key] = TileCalibTools.getTilePrefix()+folder
            self.offline_tag[key]    = offline_tag
            self.online_folder[key]  = TileCalibTools.getTilePrefix(ofl=False)+folder
            self.online_tag[key]     = ""
        
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
        for key in ['Digi','autocr']:
            self.blobWriterOnline[key]  = TileCalibTools.TileBlobWriter(self.db,self.online_folder[key], 'Flt', False)
            self.blobWriterOffline[key] = TileCalibTools.TileBlobWriter(self.db,self.offline_folder[key], 'Flt', True)
            
        
        
    def ProcessStop(self):
        # iov until is the end of the interval of validity, so infinity here
        iovUntil = (MAXRUN,MAXLBK)
        author   = "%s through Tucs" % os.getlogin()
        
        for key in ['Digi','autocr']:
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
                for p in self.parameters:
                    if p in ['ped','hfn','lfn']:
                        key = 'Digi'
                    else:
                        key = 'autocr'
                        
                    try:
                        value = event.data[p]
                    except:
                        continue

                    if value:
                        blobObjVersion = 1
                        modBlob = self.blobWriterOnline[key].getDrawer(part, mod-1)
                        if modBlob:
                            modBlob.init(self.defVec, 48,blobObjVersion)
                            modBlob = self.blobWriterOnline[key].getDrawer(part, mod-1)
                            modBlob.setData(chan, gain, self.indexDB[p], float(value))

                        modBlob = self.blobWriterOffline[key].getDrawer(part, mod-1)
                        if modBlob:
                            modBlob.init(self.defVec, 48,blobObjVersion)
                            modBlob = self.blobWriterOffline[key].getDrawer(part, mod-1)
                            modBlob.setData(chan, gain,  self.indexDB[p], float(value))
        
                
