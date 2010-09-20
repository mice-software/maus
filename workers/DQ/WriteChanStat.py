# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *
import logging, os, sys
import _mysql
import urllib
from TileCalibBlobPython import TileCalibTools, TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

def get_latest_run():
        """
        Usage: get_latest_run()

        Arguments:
        
          None
        
        Returns:
        
          * integer indicating the latest run
        """
        
        f = urllib.urlopen('http://atlas-service-db-runlist.web.cern.ch/atlas-service-db-runlist/cgi-bin/latestRun.py')
        rv = f.read()
        f.close()
        return int(rv)
    
class WriteChanStat(ReadGenericCalibration):
    "The Channel Status DB Writer to tileSqlite.db"

    def __init__(self, tag='HLT-UPD1-00', runType='all',\
                 run = 0):
        self.tag = tag
        self.runType = runType
        self.db = None
        self.mgr = None

        if run != 0:
            self.fixedRun = True
            self.runNumber = run
        else:
            self.fixedRun = False

    def ProcessStart(self):
        self.latestRun = get_latest_run()
        print 'The latest run is', get_latest_run()

        logger = getLogger("TileCalibTools")
        logger.setLevel(logging.error)

        self.db = TileCalibTools.openDb('SQLITE', 'COMP200', 'UPDATE')
        if not self.db:
            print "WriteChanStat: Failed to open a connection to the COOL SQLite3 database"
        else:
            self.folder = TileCalibTools.getTilePrefix()+"STATUS/ADC"
            self.folderTag = TileCalibUtils.getFullTag(self.folder, self.tag)
            self.mgr = TileBchTools.TileBchMgr()
            self.mgr.setLogLvl(logging.WARNING)

            if self.fixedRun:
                run = self.runNumber
            else:
                run = self.latestRun
                        
            self.mgr.initialize(self.db, self.folder, self.folderTag, (run, 0))

            if not self.mgr:
                print "WriteChanStat: Failed to create a Bad Channel manager from the DB"

            
    def ProcessStop(self):
        author   = "%s" % os.getlogin()

        if self.db and self.mgr:
            if self.fixedRun:
                run = self.runNumber
            else:
                run = self.latestRun

            comment = "Created by TUCS using the command %s" % (" ".join(sys.argv))
            self.mgr.commitToDb(self.db, self.folder, self.folderTag,\
                                TileBchDecoder.BitPat_ofl01, author, comment, (run,0))
            self.db.closeDatabase()
                

    def ProcessRegion(self, region):
        for event in region.GetEvents():
            if event.runType != self.runType and self.runType != 'all':
                continue

            if not self.db or not self.mgr:
                return

            if 'gain' in region.GetHash():
                setbit = False
                if event.data.has_key('goodRegion') and \
                   not event.data['goodRegion']:
                    setbit = True

                [x, y, z, w] = region.GetNumber()

                type = None
                if event.runType == 'CIS':
                    type = TileBchPrbs.NoCis
                elif event.runType == 'Laser':
                    type = TileBchPrbs.NoLaser
                elif event.runType == 'Cesium':
                    type = TileBchPrbs.NoCesium
                elif event.runType == 'Timing':
                    type = TileBchProbs.NoTiming
                                                
                if type:
                    if setbit:
                        self.mgr.addAdcProblem(x, y-1, z, w, type)
                        return # this will break things for many run types at same time
                    else:
                        self.mgr.delAdcProblem(x, y-1, z, w, type)
                        return
                        
                else:
                    print 'Unknown runtype ', event.runType


                    
                
