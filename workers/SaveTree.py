from src.GenericWorker import *
import cPickle as pickle

class SaveTree(GenericWorker):
    "Save tree"

    def __init__(self, filename='/tmp/dump.p'):
        self.filename = filename

    def HandleDetector(self, detector):
        pickle.dump(detector, open(self.filename, 'w'), 2)
        return detector
                      
