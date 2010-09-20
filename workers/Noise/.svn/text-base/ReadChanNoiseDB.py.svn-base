# ReadChanNoiseDB.py
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

class ReadChanNoiseDB(ReadGenericCalibration):
    '''Class to read channel noise values from conditions database'''
    
    def __init__(self,useSqlite=True,load_efit=True,load_of1=True,load_of2=False):
        self.ftDict = {} # Each element is a [TTree, TFile]
        self.useSqlite = useSqlite
        self.load_efit = load_efit
        self.load_of1  = load_of1
        self.load_of2  = load_of2
    
    def get_index(self, part, mod, chan, gain):
        return part  *64*48*2\
            + mod      *48*2\
            + chan        *2\
            + gain

    def ProcessStart(self):
        # Turn off the damn TileCalibTools startup prompt. grr.
        logger = getLogger("TileCalibTools")
        logger.setLevel(logging.error)
        
        # Open up a DB connection
        self.db = None
        if self.useSqlite:
            if not os.path.exists('tileSqlite.db'):
                print 'ReadChanNoiseDB: Failed to find tileSqlite.db'
                exit

            self.db = TileCalibTools.openDb('SQLITE', 'COMP200', 'READONLY')
        else:
            self.db = TileCalibTools.openDb('ORACLE', 'COMP200', 'READONLY')
        
        if not self.db:
            print "ReadChanNoiseDB: Failed to open a connection to the COOL ORACLE database"
        else:
            self.blobReader = {}
            self.dataName   = {}
            if self.load_efit:
                tagChan    = 'TileOfl01NoiseFit-HLT-UPD1-01'
                folderChan = TileCalibTools.getTilePrefix()+'NOISE/FIT'
                self.blobReader['efit'] = TileCalibTools.TileBlobReader(self.db, folderChan, tagChan)
                self.dataName['efit']   = 'efit_mean'
            if self.load_of1:
                tagChan    = 'TileOfl01NoiseOf1-HLT-UPD1-01'
                folderChan = TileCalibTools.getTilePrefix()+'NOISE/OF1'
                self.blobReader['of1'] = TileCalibTools.TileBlobReader(self.db, folderChan, tagChan)
                self.dataName['of1']   = 'eopt_mean'
            if self.load_of2:
                tagChan    = 'TileOfl01NoiseOf2-HLT-UPD1-01'
                folderChan = TileCalibTools.getTilePrefix()+'NOISE/OF2'
                self.blobReader['of2'] = TileCalibTools.TileBlobReader(self.db, folderChan, tagChan)
                self.dataName['of2']   = 'eopt2_mean'

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
                        event.data[self.dataName[key]+'_db']   = blob.getData(chan, gain, 0)
                        
        region.SetEvents(newevents)
