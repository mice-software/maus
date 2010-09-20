# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import os, datetime

class CISFlagProcedure(GenericWorker):
    "Run the CIS flagging procedure to mark problems"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self, dbCheck=True):
        self.dir = getPlotDirectory()
        self.dbCheck = dbCheck


    def ProcessStart(self):
        # This number is the required number of calibrations
        # before we consider a channel good.
        self.requiredCalibs = 1
                       
        self.nCalTotal = 0
        self.nCalDigitalErrors = 0
        self.nCalLargeRMS = 0
        self.nCalChi2Low = 0
        self.nCalMaxPoint = 0
        self.nCalLikelyCalib = 0
        self.nCalDeviation = 0

        self.nChanTotal = 0
        self.nChanDigitalErrors = 0
        self.nChanLargeRMS = 0
        self.nChanChi2Low = 0
        self.nChanMaxPoint = 0
        self.nChanLikelyCalib = 0
        self.nChanDeviation = 0
        self.nChanStatistics = 0 
        
        self.nGood = 0
        self.nBad = 0
        
    def ProcessStop(self):
        print 'Calibrations'
        print '\tTotal:                              ', self.nCalTotal
        print '\tPassing digital error cut:          ', self.nCalDigitalErrors, self.nCalTotal-self.nCalDigitalErrors
        print '\tPassing large RMS cut:              ', self.nCalLargeRMS, self.nCalDigitalErrors - self.nCalLargeRMS
        print '\tPassing low Chi2 cut:               ', self.nCalChi2Low, self.nCalLargeRMS - self.nCalChi2Low
        print '\tPassing max. value in fit range cut:', self.nCalMaxPoint, self.nCalChi2Low - self.nCalMaxPoint
        print '\tPassing likely calib cut:           ', self.nCalLikelyCalib, self.nCalMaxPoint - self.nCalLikelyCalib
        print '\tPassing <1% deviation cut:          ', self.nCalDeviation, self.nCalLikelyCalib - self.nCalDeviation
        
        print 'ADC Channels'
        print '\tTotal:                              ', self.nChanTotal
        print '\tPassing digital error cut:          ', self.nChanDigitalErrors, self.nChanTotal-self.nChanDigitalErrors
        print '\tPassing large RMS cut:              ', self.nChanLargeRMS, self.nChanDigitalErrors - self.nChanLargeRMS
        print '\tPassing low Chi2 cut:               ', self.nChanChi2Low, self.nChanLargeRMS - self.nChanChi2Low
        print '\tPassing max. value in fit range cut:', self.nChanMaxPoint, self.nChanChi2Low - self.nChanMaxPoint
        print '\tPassing likely calib cut:           ', self.nChanLikelyCalib, self.nChanMaxPoint - self.nChanLikelyCalib
        print '\tPassing <1% deviation cut:          ', self.nChanDeviation, self.nChanLikelyCalib - self.nChanDeviation
        
        print
        print 'Good Regions (>', self.requiredCalibs, 'successful calibrations):', self.nGood
        print 'Bad Regions: ', self.nBad

        print 'For wiki:'
        print '| %s | %d | %d (-%d) | %d (-%d) | %d (-%d) | %d (-%d) | %d (-%d) | %d (-%d) | %d |' %\
              (datetime.date.today().strftime("%d/%m/%y"), self.nBad, \
               self.nChanDeviation, self.nChanLikelyCalib - self.nChanDeviation,\
               self.nChanLikelyCalib, self.nChanMaxPoint - self.nChanLikelyCalib,\
               self.nChanMaxPoint, self.nChanChi2Low - self.nChanMaxPoint,\
               self.nChanChi2Low, self.nChanLargeRMS - self.nChanChi2Low,\
               self.nChanLargeRMS, self.nChanDigitalErrors - self.nChanLargeRMS,\
               self.nChanDigitalErrors, self.nChanTotal-self.nChanDigitalErrors,\
               self.nChanTotal
               )
        
    
    def ProcessRegion(self, region):
        # Only look at each gain within some channel
        if 'gain' not in region.GetHash():
            return

        newevents = set()

        # Decrement this number.  This number is the required number of calibrations
        # before we consider a channel good.
        numberCalibs = self.requiredCalibs
        max_progress = 0
        
        for event in region.GetEvents():
            goodEvent = False
            progress = 0
            if event.runType == 'CIS':
                assert(event.data.has_key('problems'))
                assert(isinstance(event.data['problems'], dict))

                problems = event.data['problems']
                
                progress = 1
                self.nCalTotal += 1
                if not problems['Digital Errors']:
                    progress = 2
                    self.nCalDigitalErrors += 1

                    if not problems['Large Injection RMS']:
                        progress = 3
                        self.nCalLargeRMS += 1

                        if not problems['Low Chi2']:
                            progress = 4
                            self.nCalChi2Low += 1

                            if not problems['Fail Max. Point']:
                                progress = 5
                                self.nCalMaxPoint += 1

                                if not problems['Fail Likely Calib.']:
                                    progress = 6
                                    self.nCalLikelyCalib += 1
                                    
                                    if (problems.has_key('DB Deviation') and\
                                           not problems['DB Deviation']) or not self.dbCheck:
                                        progress = 7
                                        self.nCalDeviation += 1
                                        numberCalibs -= 1
                                        goodEvent = True
                                else:
                                    pass # fail likely calib
                            else:
                                pass # fail max point
                        else:
                            #event.data['dumpScan'] = True
                            pass # fail chi2 low
                    else:
                        #event.data['dumpScan'] = True
                        #print region.GetHash()
                        pass # fail rms
                else:
                    #event.data['dumpScan'] = True
                    #print region.GetHash(), event.data['nDigitalErrors'], problems['Large Injection RMS'], event.data['scan']
                    #c1 = ROOT.TCanvas()
                    #event.data['scan'].Draw("ALP")
                    #c1.Print("blah%s.eps" % region.GetHash())
                                            
                    pass # fail digital errors

            event.data['goodEvent'] = goodEvent
            event.data['flag_progress'] = progress
            if progress >max_progress:
                max_progress = progress

            newevents.add(event)

        if max_progress >= 1:
            self.nChanTotal += 1
        if max_progress >= 2:
            self.nChanDigitalErrors += 1
        if max_progress >= 3:
            self.nChanLargeRMS += 1
        if max_progress >= 4:
            self.nChanChi2Low += 1
        if max_progress >= 5:
            self.nChanMaxPoint += 1
        if max_progress >= 6:
            self.nChanLikelyCalib += 1
        if max_progress >= 7:
            self.nChanDeviation += 1

#        if max_progress == 6:
#            print 'fail 1%', region.GetHash()

        newevents2 = set()

        for event in newevents:
            if numberCalibs <= 0:
                event.data['goodRegion'] = True
            else:
                event.data['goodRegion'] = False

            if max_progress < 7:
                event.data['moreInfo'] = True
            else:
                if not event.data.has_key('moreInfo'):
                    event.data['moreInfo'] = False
                else:
                    event.data['moreInfo'] = event.data['moreInfo'] | False
                    
            newevents2.add(event)

        if numberCalibs <= 0:
            self.nGood += 1
        else:
#            print 'bad%d' % max_progress, region.GetHash(), region.GetHash(1)
            self.nBad += 1

        region.SetEvents(newevents2)

        
        
