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

class CopyLocalChanStat(ReadGenericCalibration):
    "Make a local copy of the Channel Status DB to tileSqlite.db"

                                
    def ProcessStart(self):
        # It would be nice if this wasn't a system call...
        
        if os.path.exists('tileSqlite.db'):
            os.unlink('tileSqlite.db')

        print "CopyLocalChanStat: Warning: I'm about to do a system call to AtlCoolCopy.exe.  This is dangerous, and I should feel ashamed.  Check 'db_copy_log' for the output."
        
        os.system("""AtlCoolCopy.exe "COOLONL_TILE/COMP200" "sqlite://;schema=tileSqlite.db;dbname=COMP200" -create -copytaglock  -t TileOfl01StatusAdc-HLT-UPD1-00 -f /TILE/OFL01/STATUS/ADC 2>&1 1>db_copy_log""")

    def ProcessRegion(self, region):
        pass
