#!/bin/env python
# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#
import os, sys, gc
#from src.region import * # remove?
from datetime import datetime

import logging

logging.basicConfig(level=logging.INFO)

logger_core_go = logging.getLogger('core.go')

class Go:
    ## Iterate through the workers
    #
    # this processor handles all the workers
    def __init__(self, workers):
        logger_core_go.info('Welcome to MAUS (pid %d).  Loading configuration file... ' % os.getpid())
        startup = datetime.today()
        
        #self.detector = constructTileCal()
        logger_core_go.info('done!\
                             \
                             Entering worker loop:')

        if workers:
            for worker in workers:
                if worker:
                    print 'Running %s - %s' % (worker.__class__.__name__, worker.__class__.__doc__)
                    sys.stdout.flush()
                    #self.detector = worker.HandleDetector(self.detector)
                    gc.collect()
                    #if not isinstance(self.detector, Region):
                    #    print "The following worker returned non-sense:", worker
                    #    break

        # free up memory
        #for region in self.detector.RegionGenerator():
        #    region.SetEvents(set())
        #    region.SetChildren(set())
        #del self.detector
        #gc.collect(2)
        print
        print 'MAUS finished in:', datetime.today() - startup
