# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#
# Changed by Mikhail Makouski <makouski@mail.cern.ch>
# June 2009

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

class ReadIntegratorDB(ReadGenericCalibration):
    "Read database constants from a TileBlob"

    def __init__(self, useSqlite=False, runType='cesium',intgain=3):
        self.useSqlite = useSqlite
        self.runType = runType
        self.gain=intgain
        self.refgain=28.85
            
    def ProcessStart(self):
        logger = getLogger("TileCalibTools")
        logger.setLevel(logging.error)

        # Open up a DB connection
        self.db = None
        if self.useSqlite:
            if not os.path.exists('tileSqlite.db'):
                print 'ReadIntegratorDB: Failed to find tileSqlite.db'

            self.db = TileCalibTools.openDb('SQLITE', 'COMP200', 'READONLY')
        else:
            self.db = TileCalibTools.openDbConn('COOLOFL_TILE/COMP200', 'READONLY')

        if not self.db:
            print "ReadDB: Failed to open a database connection"
        else:
            if self.runType == 'cesium':
                self.tag         = "TileOfl02Integrator-HLT-UPD1-00"
                self.intfolder   = "/TILE/OFL02/INTEGRATOR"
                self.cstag       = "TileOfl02CalibCes-HLT-UPD1-01"
                self.csfolder    = "/TILE/OFL02/CALIB/CES"
            else:
                print "ReadDB: Don't know the tag or folder for runType %s" % self.runType
                self.tag = ''
                self.intfolder = ''
            self.blobReader = TileCalibTools.TileBlobReader(self.db, self.intfolder, self.tag)
            if self.runType == 'cesium':
                self.csblobReader=TileCalibTools.TileBlobReader(self.db, self.csfolder, self.cstag)
            

    def ProcessStop(self):
        if self.db:
            print 'ReadIntegratorDB: Closing database'
            self.db.closeDatabase()
    
    def ProcessRegion(self, region):
        #newevents = set()
        for event in region.GetEvents():
            # Query the database for integrator constants for given region
            if event.runType == 'cesium':
                if self.db:
                    if self.blobReader:
                        if not event.data.has_key('calibration'):
                            #print 'ReadDB: No calibration for %s' % region.GetHash()
                            continue
                            
                        [x, y, z] = region.GetNumber()
                        #if self.runNumber == '':
                        #    intDrawer = self.blobReader.getDrawer(x, y-1, (event.runNumber, 0))
                        #else:
                        intDrawer = self.blobReader.getDrawer(x, y-1, (event.runNumber, 0))
                        csDrawer  = self.csblobReader.getDrawer(x, y-1, (event.runNumber, 0))
                        #print 'integrator ',intDrawer.getData(z, self.gain-1, 0)
                        event.data['integrator'] = intDrawer.getData(z, self.gain-1, 0)/self.refgain
                        if event.data['integrator']<0.5:
                            print "bad integrator gain", event.data['integrator'],x,y,z
                            event.data['integrator']=None
                            print "default",csDrawer.getData(z, 0, 0)
                        event.data['csDB'] = csDrawer.getData(z, 0, 0)
                        
            #newevents.add(event)

        #region.SetEvents(newevents)

