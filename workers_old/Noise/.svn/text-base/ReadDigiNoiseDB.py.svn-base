# ReadDigiNoiseDB.py
# Class for reading digital noise from DB
# Author: Brian Martin (brian.thomas.martin@cern.ch)

from src.ReadGenericCalibration import *
from src.region import *
from TileCalibBlobPython import TileCalibTools, TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

#=== get a logger
import logging
from TileCalibBlobPython.TileCalibLogger import TileCalibLogger, getLogger

class ReadDigiNoiseDB(ReadGenericCalibration):
    '''Class to read digital noise values from conditions database'''
    
    def __init__(self,useSqlite=True,load_autocr=True):
        self.ftDict = {} # Each element is a [TTree, TFile]
        self.useSqlite = useSqlite
        self.load_autocr = load_autocr
    
    def get_index(self, part, mod, chan, gain):
        return part  *64*48*2\
            + mod      *48*2\
            + chan        *2\
            + gain

    def ProcessStart(self):
        loggerTCT = getLogger("TileCalibTools")
        loggerTCT.setLevel(logging.error)
        
        # Open up a DB connection
        self.db = None
        if self.useSqlite:
            if not os.path.exists('tileSqlite.db'):
                print 'ReadDigiNoiseDB: Failed to find tileSqlite.db'
                exit

            self.db = TileCalibTools.openDb('SQLITE', 'COMP200', 'READONLY')
        else:
            self.db = TileCalibTools.openDb('ORACLE', 'COMP200', 'READONLY')

        if not self.db:
            print "ReadDigiNoiseDB: Failed to open a connection to the COOL ORACLE database"
        else:
            self.blobReader = {}
            tagDigi         = 'TileOfl01NoiseSample-HLT-UPD1-01'
            folderDigi      = TileCalibTools.getTilePrefix()+'NOISE/SAMPLE'
            self.blobReader['Digi'] = TileCalibTools.TileBlobReader(self.db, folderDigi, tagDigi)
            if(self.load_autocr):
                tagDigi         = 'TileOfl01NoiseAutocr-HLT-UPD1-00'
                folderDigi      = TileCalibTools.getTilePrefix()+'NOISE/AUTOCR'
                self.blobReader['autocr'] = TileCalibTools.TileBlobReader(self.db, folderDigi, tagDigi)
            

    def ProcessStop(self):
        if self.db:
            self.db.closeDatabase()

    def ProcessRegion(self, region):
        newevents = set()
        # only interested in channel or adc level
        if 'gain' not in region.GetHash():
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
                
        for event in newevents:
            # Get indices 
            [part, mod, chan, gain] = region.GetNumber()
            
            # Query the database for the current Noise constants
            if self.db:
                if self.blobReader:
                    for key in self.blobReader.keys():
                        blob = self.blobReader[key].getDrawer(part, mod-1, (event.runNumber, 0))  # DB module numbers start from 0
                        if key == 'Digi':
                            event.data['ped_db']   = blob.getData(chan, gain, 0)
                            event.data['hfn_db']   = blob.getData(chan, gain, 1)
                            #event.data['lfn_db']   = blob.getData(chan, gain, 2)
                        elif key =='autocr':
                            for i in xrange(6):
                                event.data['autocorr'+str(i)+'_db']   = blob.getData(chan, gain, i)
                        
        region.SetEvents(newevents)
