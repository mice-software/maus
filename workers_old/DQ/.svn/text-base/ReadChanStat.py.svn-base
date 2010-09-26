# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.ReadGenericCalibration import *
from src.region import *
import logging, os, sys
from TileCalibBlobPython import TileCalibTools, TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

# TODO.  this isn't compliant with the new processregion way.

class ReadChanStat(ReadGenericCalibration):
    "The ChanStat Reader"

    use_oracle = True
    db = None
    mgr = None

    def ProcessStart(self):
        # Setup the DB if we want to grab calibration facotrs
        # this DB crap should be elsewhere fixme todo
        logger = getLogger("TileCalibTools")
        logger.setLevel(logging.error)

        folder = TileCalibTools.getTilePrefix()+'STATUS/ADC'
        folderTag = TileCalibUtils.getFullTag(folder, 'HLT-UPD1-00')
        
        if self.use_oracle:
            self.db = TileCalibTools.openDb('ORACLE', 'COMP200', 'READONLY')
        else:
            self.db = TileCalibTools.openDb('SQLITE', 'COMP200', 'UPDATE')
            
        if not self.db:
            print "ReadCISFile: Failed to open a connection to the COOL ORACLE database"
        else:
            self.mgr = TileBchTools.TileBchMgr()
            logger = getLogger("TileBchMgr")
            logger.setLevel(logging.error)
                            
            self.mgr.initialize(self.db, folder, folderTag)

        if not self.mgr:
            print "ReadChanStat: Failed to create a Bad Channel manager from the DB"
            
        def ProcessStop(self):
            if self.db:
                self.db.closeDatabase()
                

    def ProcessRegion(self, region):
        newevents = set()
        for event in region.GetEvents():
            if event.runType != 'staging':
                newevents.add(event)
                continue
        
            if not self.db or not self.mgr:
                return

            if 'gain' in region.GetHash():
                [x, y, z, w] = region.GetNumber()
                data = {}

                # The database uses module numbers starting from 0!
                problems = self.mgr.getAdcProblems(x, y - 1, z, w)

                if len(problems) != 0:
                    data['problems'] = set()
                    for prbCode in sorted(problems.keys()):
                        data['problems'].add(problems[prbCode])
                        
                    data['isBad'] = True
                    newevents.add(Event('ChanStat', event.runNumber, data, event.time))
                else:
                    data['isBad'] = False
                    data['problems'] = set()
                    newevents.add(Event('ChanStat', event.runNumber, data, event.time))

        region.SetEvents(newevents)
                

