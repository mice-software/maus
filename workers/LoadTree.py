from src.GenericWorker import *
import cPickle as pickle

class LoadTree(GenericWorker):
    "Load tree"

    def __init__(self, filename='/tmp/dump.p'):
        self.filename = filename

    def HandleDetector(self, detector):
        del detector
        detector = pickle.load(open(self.filename, 'rb'))
        return detector
                      
