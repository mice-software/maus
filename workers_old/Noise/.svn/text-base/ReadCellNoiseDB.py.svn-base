# ReadCellNoiseDB.py
# Class for reading cell noise from DB
# Author: Brian Martin (brian.thomas.martin@cern.ch)

from src.ReadGenericCalibration import *
from src.region import *
from CaloCondBlobAlgs import CaloCondTools
from CaloCondBlobAlgs.CaloCondLogger import CaloCondLogger, getLogger

class ReadCellNoiseDB(ReadGenericCalibration):
    '''Class to read cell noise values from conditions database'''
    
    def __init__(self,useSqlite=True):
        self.useSqlite=useSqlite
        self.type='physical'
        #self.gainStr = ['LGLG','LG--','--LG','HGHG','HG--','--HG']
        self.gainStr = [ 'LGLG','LGHG','HGLG','HGHG' ]

    def ProcessStart(self):
        loggerCCT = getLogger("CaloCondTools")
        loggerCCT.setLevel(logging.WARNING)
        
        # Open up a DB connection
        self.db = None
        if self.useSqlite:
            if not os.path.exists('caloSqlite.db'):
                print 'ReadDB: Failed to find caloSqlite.db'
                exit

            self.db = CaloCondTools.openDb('SQLITE', 'COMP200', 'READONLY')
        else:
            self.db = CaloCondTools.openDb('ORACLE', 'COMP200', 'READONLY')
        
        if not self.db:
            print "ReadNoise: Failed to open a connection to the COOL database"
        else:
            tagCell         = 'CaloNoiseCellnoise-UPD3-00'
            folderCell      = CaloCondTools.getCaloPrefix()+'Noise/CellNoise'
            self.blobReader = CaloCondTools.CaloBlobReader(db=self.db, folder=folderCell, tag=tagCell)

    def ProcessStop(self):
        if self.db:
            self.db.closeDatabase()

    def ProcessRegion(self, region):
        newevents = set()
        # only interested in Cell level
        if '_t' not in region.GetHash():
            for event in region.GetEvents():
                if event.runType != 'staging':
                    newevents.add(event)
            region.SetEvents(newevents)
            return

        # check if Ped event exists (if so, use them). if staging events exist, convert to ped
        for event in region.GetEvents():
            if event.runType == 'Ped':
                newevents.add(event)
            if event.runType == 'staging':
                newevents.add(Event('Ped', event.runNumber, event.data, event.time))
            
        if len(newevents):
            # Get indices 
            [part, module, sample, tower] = region.GetNumber()
            detectorId = 48 # Cool det. ID for TileCal
            cellHash = CaloCondTools.getCellHash(detectorId,part,module-1,sample,tower)
            
        for event in newevents:
            # Query the database for the current Noise constants
            if self.db:
                if self.blobReader:
                    blob = self.blobReader.getCells(detectorId,(event.runNumber, 0))  # DB module numbers start from 0
                    for gain in xrange(4):
                        event.data['cellnoise'+self.gainStr[gain]+'_db']   = blob.getData(cellHash,gain, 0)
                        event.data['pilenoise'+self.gainStr[gain]+'_db']   = blob.getData(cellHash,gain, 1)
                        #event.data['sigma1'+self.gainStr[gain]+'_db']   = blob.getData(cellHash,gain, 2)
                        #event.data['sigma2'+self.gainStr[gain]+'_db']   = blob.getData(cellHash,gain, 3)
                        #event.data['normal'+self.gainStr[gain]+'_db']   = blob.getData(cellHash,gain, 4)
        
        region.SetEvents(newevents)
