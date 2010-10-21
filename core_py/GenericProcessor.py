# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#
from core.Map import Map

class GenericProcessor(object):
    "Generic Worker Class"

    # Constructor
    def __init__(self, verbose=False):
        if verbose:
            print "Print class set to verbose"
        self.verbose = verbose


    def ProcessStart(self):
        pass

    def Process(self, general, run, spill, event):
        assert(isinstance(general, Map))
        assert(isinstance(run, Map))
        assert(isinstance(spill, Map))
        assert(isinstance(event, Map))
        pass

    def ProcessStop(self):
        pass
