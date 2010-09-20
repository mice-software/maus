# Author: Sebastien Viret <viret@in2p3.fr>
#
# April 20, 2009
#
# This macro get the laser info from the calibration ROOTuple stored on lxplus
#

from src.ReadGenericCalibration import *
import random

class ReadLaser(ReadGenericCalibration):
    "The Laser Calibration Reader"

    processingDir = None
    numberEventCut = None
    ftDict = None

    def __init__(self, processingDir='/afs/cern.ch/user/t/tilecali/w0/ntuples/las'):
        self.processingDir = processingDir
        self.numberEventCut = 1000
        self.ftDict = {}


from src.ReadGenericCalibration import *
import random

class ReadLaser(ReadGenericCalibration):
    "The Laser Calibration Reader"

    processingDir = None
    numberEventCut = None
    ftDict = None

    def __init__(self, processingDir='/afs/cern.ch/user/t/tilecali/w0/ntuples/las'):
        self.processingDir = processingDir
        self.numberEventCut = 1000
        self.ftDict = {}

    def get_index(self, ros, mod, chan, gain):
        return ros  *64*48*2\
            + mod      *48*2\
            + chan        *2\
            + gain

    def get_index(self, ros, mod, chan, gain):
        return ros  *64*48*2\
            + mod      *48*2\
            + chan        *2\
            + gain

    def ProcessRegion(self, region):
        if 'gain' in region.GetHash():
            for event in region.GetEvents():
                if event.runNumber and event.runType == 'staging' and\
                       not self.ftDict.has_key(event.runNumber):
                    f, t = self.getFileTree('tileCalibLAS_%s_Las.0.root' % event.runNumber, 'h3000')

                    if [f, t] == [None, None]:
                        f, t = self.getFileTree('tileCalibLAS_%s.root' % event.runNumber, 'h3000')

                        if [f, t] == [None, None]:
                            print "I tried everything..."
                            continue

                    self.ftDict[event.runNumber] = [f, t]

        newevents = set()
        for event in region.GetEvents():
            # If we don't care about the event, pass it on
            if event.runType != 'staging':
                newevents.add(event)

            if 'gain' not in region.GetHash():
                continue

            if not self.ftDict.has_key(event.runNumber):
                continue
            
            f,t = self.ftDict[event.runNumber]
            t.GetEntry(0)

            [x, y, z, w] = region.GetNumber()
            index = self.get_index(x-1, y-1, z, w)

            data = {}

            data['wheelpos']       = t.wheelpos  # Filter wheel position
            data['requamp']        = t.requamp   # Requested amplitude


            data['number_entries'] = t.n_LASER_entries[index]  # Number of entries for the channel
            data['calib_const']    = t.signal_cor[index]       # Calibration value (PMT/Diode) for the channel
            data['slope']          = t.signal_slope[index]     # Slope measured for the ratio during the run (in h-1)
            data['region']         = region.GetHash()          # Channel position

            # Here we proceed to some selection cuts (SV: STILL PRELIMINARY)
          
            if data['number_entries'] >= self.numberEventCut and\
                   data['calib_const'] > 0:
                newevents.add(Event('Laser', event.runNumber, data, None))

        region.SetEvents(newevents)
                    

    def setNumberEventCut(self, numberEventCut):
        self.numberEventCut = numberEventCut

    def ProcessStop(self):
        self.ftDict = {}
