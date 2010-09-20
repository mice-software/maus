# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 17, 2009
#

from src.ReadGenericCalibration import *
from src.region import *
import logging, os, sys
from TileCalibBlobPython import TileCalibTools, TileBchTools
from TileCalibBlobPython.TileCalibTools import MINRUN, MINLBK, MAXRUN, MAXLBK
from TileCalibBlobObjs.Classes import *

class CopyDBConstants(ReadGenericCalibration):
    "Make a local copy of the DB constants to tileSqlite.db"

    def __init__(self, runType, recreate=False):
        self.runType = runType
        self.recreate = recreate
                                
    def ProcessStart(self):
        # It would be nice if this wasn't a system call...
        
        if os.path.exists('tileSqlite.db') and self.recreate: 
            os.unlink('tileSqlite.db')

        print "CopyDBConstants: Warning: I'm about to do a system call to AtlCoolCopy.exe.  This is dangerous, and I should feel ashamed.  Check 'db_copy_constants_log' for the output."

        folder = None
        tag = None
        if self.runType == 'CIS':
            folder = '/TILE/OFL01/CALIB/CIS/FIT/LIN'
            tag = 'TileOfl01CalibCisFitLin-HLT-UPD1-00'
            
        command = """AtlCoolCopy.exe "COOLONL_TILE/COMP200" "sqlite://;schema=tileSqlite.db;dbname=COMP200" -create -copytaglock -f %s -t %s 2>&1 1>db_copy_constants_log""" % (folder,tag)
        print 'running: ', command
        os.system(command)

    def ProcessRegion(self, region):
        pass
