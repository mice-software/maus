# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

import os
from ROOT import TFile
from src.GenericWorker import *

class ReadGenericCalibration(GenericWorker):
    "The Generic Calibration Template"

    tfile_cache = {}
    processingDir = 'tmp'
    
    def getFileTree(self, fileName, treeName):
        f, t = [None, None]

        if self.tfile_cache.has_key(fileName):
            f, t = self.tfile_cache[fileName]
        else:
            if os.path.exists(os.path.join(self.processingDir, fileName)) or 'rfio:/' == self.processingDir[0:6]:
                f = TFile.Open(os.path.join(self.processingDir, fileName), "read")

        if not f:
            return [None, None]
            
        t = f.Get(treeName)
        if not t:
            print "Tree failed to be grabbed: " + fileName
            return [None, None]

        self.tfile_cache[fileName] = [f, t]
        return [f, t]
