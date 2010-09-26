# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

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

class ReadDB(ReadGenericCalibration):
    "Read database calibration constants from COOL"

    def __init__(self, useSqlite=False, runType='CIS', runNumber = ''):
        self.useSqlite = useSqlite
        self.runType = runType
        self.runNumber = runNumber
    
    def ProcessStart(self):
        # Turn off the damn TileCalibTools startup prompt. grr.
        logger = getLogger("TileCalibTools")
        logger.setLevel(logging.error)

        # Open up a DB connection
        self.db = None
        if self.useSqlite:
            if not os.path.exists('tileSqlite.db'):
                print 'ReadDB: Failed to find tileSqlite.db'

            self.db = TileCalibTools.openDb('SQLITE', 'COMP200', 'READONLY')
        else:
            self.db = TileCalibTools.openDb('ORACLE', 'COMP200', 'READONLY')

        if not self.db:
            print "ReadDB: Failed to open a database connection"
        else:
            if self.runType == 'CIS':
                self.tag         = "TileOfl01CalibCisFitLin-HLT-UPD1-00"
                self.folderLin = TileCalibTools.getTilePrefix()+"CALIB/CIS/FIT/LIN"
            else:
                print "ReadDB: Don't know the tag or folder for runType %s" % self.runType
                self.tag = ''
                self.folderLin = ''
            self.blobReader = TileCalibTools.TileBlobReader(self.db, self.folderLin, self.tag)

    def ProcessStop(self):
        if self.db:
            self.db.closeDatabase()
    
    def ProcessRegion(self, region):
        newevents = set()
        for event in region.GetEvents():
            # Querty the database for the CIS constant currently in there
            if event.runType == 'CIS' or event.runType == 'Laser':
                if self.db:
                    if self.blobReader:
                        if not event.data.has_key('calibration'):
                            print 'ReadDB: No calibration for %s' % region.GetHash()
                            
                        [x, y, z, w] = region.GetNumber()
                        if self.runNumber == '':
                            oldDrawer = self.blobReader.getDrawer(x, y-1, (event.runNumber, 0))  # DB module numbers start from 0
                        else:
                            oldDrawer = self.blobReader.getDrawer(x, y-1, (self.runNumber, 0))
                        event.data['calibration_db'] = oldDrawer.getData(z, w, 0)
                        event.data['deviation'] =  (event.data['calibration'] - event.data['calibration_db'])/event.data['calibration_db']

                        if not event.data.has_key('problems'):
                            event.data['problems'] = {}

                        if abs(event.data['deviation']) > 0.01:
                            event.data['problems']['DB Deviation'] = True
                        else:
                            event.data['problems']['DB Deviation'] = False

                        if event.runType == 'CIS':
                            # Check for default calibration
                            if 'high' in region.GetHash() and\
                                   abs((event.data['calibration_db'] - 81.8399963379)) < 0.00000001:
                                event.data['problems']['Default Calibration'] = True
                            elif 'low' in region.GetHash() and\
                                     abs((event.data['calibration_db'] - 1.27874994278)) < 0.00000001:
                                event.data['problems']['Default Calibration'] = True
                            else:
                                event.data['problems']['Default Calibration'] = False
                                    
                            
                            
            newevents.add(event)

        region.SetEvents(newevents)
