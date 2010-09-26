# NoiseWorker.py
# Parent Class for all noise workers
# Author: Brian Martin (brian.thomas.martin@cern.ch)
from src.region import *
from src.GenericWorker import *
import logging
import math
import ROOT

# Non-class utilities
def getMean(numbers):
    if len(numbers)==0:
        return 0
    else:
        return 1.0*sum(numbers)/len(numbers)

def getMedian(numbers):
    '''Takes a list of numbers, returns the median'''
    size = len(numbers)
    sorted(numbers)
    if size==0:
        return 0
    if size==1:
        return numbers[0]
    if (size%2) == 0:
        med = (numbers[int(size/2)]+numbers[int(size/2)-1])/2
    else:
        med = numbers[int((size-1)/2)]

    return med

def getRMS(numbers):
    '''Takes list of numbers, returns RMS difference'''
    if len(numbers)==0:
        return 0
    mean = getMean(numbers)
    rms = 0.
    for num in numbers:
        rms += (num-mean)**2
    rms /= len(numbers)
    rms = math.sqrt(rms)
    
    return rms
        
class NoiseWorker(GenericWorker):
    '''Base class for noise workers'''

    def initLog(self,level=logging.CRITICAL):
        '''Set up noise logging streams: one to stdout, one to file'''
        self.noiseLog = logging.getLogger('noiseLog')
        if len(self.noiseLog.handlers) == 0:
            self.noiseLog.propagate = 0 # needed to prevent being picked up by root logger
            self.noiseLog.setLevel(logging.DEBUG)
            fileOut = logging.FileHandler('noise.out',mode='w')
            fileOut.setLevel(logging.INFO)
            stdOut  = logging.StreamHandler()
            stdOut.setLevel(level)
            self.noiseLog.addHandler(fileOut)
            self.noiseLog.addHandler(stdOut)
    
    def initNoiseHistFile(self,filename='noise.HIST.root'):
        self.noiseHistFile = ROOT.gROOT.GetFile(filename)
        try:
            self.noiseHistFile.cd()
        except:
            self.noiseHistFile = ROOT.TFile(filename,'RECREATE')
            

    def get_index(self, type, part, mod, chan, gain, gainCell=0):
        if type == 'physical':
            samp = chan
            tower= gain
            return side *64*4*17*6\
                + mod      *4*17*6\
                + samp       *17*6\
                + tower         *6\
                + gainCell
        else:
            return part  *64*48*2\
                + mod      *48*2\
                + chan        *2\
                + gain

