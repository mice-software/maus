# WriteDigiNoiseDB.py
# Class for writing cell noise to DB
# Author: Brian Martin (brian.thomas.martin@cern.ch)

from src.ReadGenericCalibration import *
from src.region import *
import os

# For reading from DB
from CaloCondBlobAlgs import CaloCondTools
from CaloCondBlobAlgs.CaloCondLogger import CaloCondLogger, getLogger

class WriteCellNoiseDB(ReadGenericCalibration):
    '''Write out cell noise constants to sqlite file'''

    def __init__(self, runType = 'PedUpdate', parameter='all',offline_iov = 'use latest run',load_autocr=False):
        self.type = 'physical'
        self.runType = runType

        if parameter == 'all':
            self.parameters = ['cellnoise'+g for g in ['LGLG','LGHG','HGLG','HGHG']]
            #self.parameters += ['pilenoise'+g for g in ['LGLG','LGHG','HGLG','HGHG']]
        else:
            self.parameters = [parameter]

        self.indexDB  = {'cellnoise':0,'pilenoise':1}
        self.gainDict = {'LGLG':0,'LGHG':1,'HGLG':2,'HGHG':3}

        self.offline_folder = CaloCondTools.getCaloPrefix()+'Noise/CellNoise'
        self.offline_tag    = 'CaloNoiseCellnoise-UPD3-00'
        #self.online_folder  = CaloCondTools.getCaloPrefix(ofl=False)+'Noise/CellNoise'
        #self.online_tag     = ""
        
        if offline_iov == 'use latest run':
            self.offline_iov_use_latest = True
            self.offline_iov = (MAXRUN,MAXLBK)
        else:
            self.offline_iov_use_latest = False
            self.offline_iov = offline_iov  # The format of this is (run number, lumi block). Example: (92929, 0)
        
        #self.online_iov = (0, 0)

    
    def ProcessStart(self):
        PyCintex.makeClass('std::vector<float>')
        
        # open DB connection to sqlite file
        self.db = CaloCondTools.openDb('SQLITE', 'COMP200', 'UPDATE')
        
        GainDefVec = PyCintex.gbl.std.vector('float')()
        GainDefVec.push_back(0.0)
        self.defVec = PyCintex.gbl.std.vector('std::vector<float>')()
        self.defVec.push_back(GainDefVec)
        self.defVec.push_back(GainDefVec)
        self.defVec.push_back(GainDefVec)
        self.defVec.push_back(GainDefVec)
        
        #self.blobWriterOnline  = CaloCondTools.CaloBlobWriter(self.db,self.online_folder, 'Flt', False)
        self.blobWriterOffline = CaloCondTools.CaloBlobWriter(self.db,self.offline_folder, 'Flt', True)
        
    def ProcessStop(self):
        # iov until is the end of the interval of validity, so infinity here
        iovUntil = (MAXRUN,MAXLBK)
        author   = "%s through Tucs" % os.getlogin()
        
        #self.blobWriterOnline.setComment(author, " ".join(sys.argv))
        #self.blobWriterOnline.register(self.online_iov, iovUntil, self.online_tag)
    
        #self.blobWriterOffline.setComment(author, " ".join(sys.argv))
        self.blobWriterOffline.register(self.offline_iov, iovUntil, self.offline_tag)
            
        self.db.closeDatabase()
            
    def ProcessRegion(self, region):
        if '_t' not in region.GetHash():
            return

        [part, module, sample, tower] = region.GetNumber()
        detectorId = 48 # Cool det. ID for TileCal
        cellHash = CaloCondTools.getCellHash(detectorId,part,module-1,sample,tower)

        value = None
        writeParam = {}
        for event in region.GetEvents():
            if event.runType == self.runType:
                for p in self.parameters:
                    writeParam[p] = False
                    try:
                        value = event.data[p]
                    except:
                        continue
                    
                    if value:
                        writeParam[p] = True

                    blobObjVersion = 1
                    #cellBlob = self.blobWriterOnline.getCells(detectorId)
                    #if cellBlob:
                    #    cellBlob.init(self.defVec, 5184,blobObjVersion)
                    #    for p in writeParam:
                    #        if writeParam[p]:
                    #            cellBlob.setData(cellHash, self.gainDict[p[-4:]], self.indexDB[p[:-4]], float(event.data[p]))

                    cellBlob = self.blobWriterOffline.getCells(detectorId)
                    if cellBlob:
                        cellBlob.init(self.defVec, 5184,blobObjVersion)
                        for p in writeParam:
                            if writeParam[p]:
                                cellBlob.setData(cellHash, self.gainDict[p[-4:]],  self.indexDB[p[:-4]], float(event.data[p]))
        
                
