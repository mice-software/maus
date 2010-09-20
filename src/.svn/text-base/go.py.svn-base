#!/bin/env python
# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#
import os, sys, gc
from src.region import *
from datetime import datetime

# This processes everything
class Go:
    def __init__(self, workers):
        print 'Welcome to TUCS (pid %d).  Building detector tree... ' % os.getpid(),
        startup = datetime.today()
        
        self.detector = constructTileCal()
        print 'done!'
        print
        print 'Entering worker loop:'

        if workers:
            for worker in workers:
                if worker:
                    print 'Running %s - %s' % (worker.__class__.__name__, worker.__class__.__doc__)
                    sys.stdout.flush()
                    self.detector = worker.HandleDetector(self.detector)
                    gc.collect()
                    if not isinstance(self.detector, Region):
                        print "The following worker returned non-sense:", worker
                        break

        # free up memory
        for region in self.detector.RegionGenerator():
            region.SetEvents(set())
            region.SetChildren(set())
        del self.detector
        gc.collect(2)
        print
        print 'TUCS finished in:', datetime.today() - startup
