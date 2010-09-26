# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import os, datetime
from array import array

class CISRecalibrateProcedure(GenericWorker):
    "Run the CIS calibration procedure to determine if a channel should be recalibrated"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self, all=False):
        self.dir = getPlotDirectory()
        self.all = all

    def ProcessStart(self):
        # This number is the required number of calibrations
        # before we consider a channel good.
        self.requiredCalibs = 3

        # calibration/event information (one per channel per run)
        self.nCalTotal = 0
        self.nCalDigitalErrors = 0
        self.nCalLargeRMS = 0
        self.nCalChi2Low = 0
        self.nCalNoResp = 0
        
        # channel information
        self.nChanTotal = 0       # step 1
        self.nChanPassQFlag = 0   # step 2
        
        self.nChanDev = 0         # step 3 OR
        self.nChanDef = 0         # still step 3
        self.nChanAll = 0
        
        self.nChanStatistics = 0      # step 4
        self.nChanStable = 0      # step 5

        self.hlo = ROOT.TH1F('low-gain', '', 20, 0, 1)
        self.hhi = ROOT.TH1F('high-gain', '', 20, 0, 1)
        self.hlo.SetLineColor(ROOT.kBlue)
        self.hhi.SetLineColor(ROOT.kRed)  
        
    def ProcessStop(self):
        print 'Calibrations'
        print '\tTotal:                    ', self.nCalTotal
        print '\tPassing digital error cut:', self.nCalDigitalErrors, self.nCalTotal-self.nCalDigitalErrors
        print '\tPassing large RMS cut:    ', self.nCalLargeRMS, self.nCalDigitalErrors - self.nCalLargeRMS
        print '\tPassing low Chi2 cut:     ', self.nCalChi2Low, self.nCalLargeRMS - self.nCalChi2Low
        print '\tPassing no response cut:  ', self.nCalNoResp, self.nCalChi2Low-self.nCalNoResp

        print
        print 'ADC Channels'
        print '\tTotal Attempts:                     ', self.nChanTotal
        print '\tPassing quality flags:             ', self.nChanPassQFlag, self.nChanTotal - self.nChanPassQFlag
        temp = self.nChanDef+self.nChanDev+self.nChanAll
        print '\tWith defaults/deviations/forced:   ', temp, '(%d/%d/%d)' % (self.nChanDef, self.nChanDev, self.nChanAll), self.nChanPassQFlag - temp
        print '\tWith >', self.requiredCalibs, 'good runs:                ', self.nChanStatistics, temp - self.nChanStatistics
        print '\tWith stable constants:             ', self.nChanStable, self.nChanStatistics - self.nChanStable

        print
        print 'Recalibrating:   ', self.nChanStable, '(%0.3f%%)' % (float(self.nChanStable)/19704)
        print 'Not recalibrating', 19704-self.nChanStable,  '(%0.3f%%)' % (float(19704-self.nChanStable)/19704)

        self.c1.SetLogy(1)
        self.c1.cd()
        self.c1.Modified()
        self.c1.Update()
        self.c1.Modified()
        xtitle = 'Calibration Factor RMS / mean (%)'
        self.hlo.GetXaxis().SetTitle(xtitle)
        self.hhi.GetXaxis().SetTitle(xtitle)
        ytitle='Number of ADC channels'
        self.hlo.GetYaxis().SetTitle(ytitle)
        self.hhi.GetYaxis().SetTitle(ytitle)

        
        if not self.hlo.GetEntries() or not self.hhi.GetEntries():
            return


        ROOT.gStyle.SetOptStat(0)
        self.c1.Modified()
        self.hlo.Draw('')
        self.hhi.Draw('SAME')

        ptstatshi = ROOT.TPaveStats(0.7,0.45,0.9,0.65,"brNDC")
        ptstatshi.SetName("statshi")
        ptstatshi.SetBorderSize(1)
        ptstatshi.SetTextAlign(12)
        text = ptstatshi.AddText("high-gain")
        text.SetTextSize(0.046);
        ptstatshi.AddText("Entries  = 9737   ")
        ptstatshi.AddText("Mean     = 0.03997")
        ptstatshi.AddText("RMS      = 0.03835")
        ptstatshi.AddText("Overflow = %d" % self.hhi.GetBinContent(self.hhi.GetNbinsX()+1))
        ptstatshi.SetOptStat(1)
        ptstatshi.SetOptFit(0)
        ptstatshi.Draw()

        ptstatslo = ROOT.TPaveStats(0.7,0.7,0.9,0.9,"brNDC")
        ptstatslo.SetName("statslo")
        ptstatslo.SetBorderSize(1)
        ptstatslo.SetTextAlign(12)
        text = ptstatslo.AddText("low-gain")
        text.SetTextSize(0.046)
        ptstatslo.AddText("Entries  = 9760   ")
        ptstatslo.AddText("Mean     = 0.03542")
        ptstatslo.AddText("RMS      = 0.02899")
        ptstatslo.AddText("Overflow = %d" % self.hlo.GetBinContent(self.hlo.GetNbinsX()+1))
        ptstatslo.SetOptStat(1)
        ptstatslo.SetOptFit(0)
        ptstatslo.Draw()


        leg = ROOT.TLegend(0.2424623,0.7814685,0.428392,0.9195804, "","brNDC")
        leg.AddEntry(self.hhi,"high-gain","l")
        leg.AddEntry(self.hlo,"low-gain","l")
        leg.Draw()
        
        self.c1.Print('%s/time_spread_rms.eps' % self.dir)
        self.c1.Print('%s/time_spread_rms.root' % self.dir)
        self.c1.SetLogy(0)
    
    def ProcessRegion(self, region):
        # Only look at each gain within some channel
        if 'gain' not in region.GetHash():
            return

        calibs = []          # Store the calibration constant for computing the mean/RMS
        goodCal  = 0         # Store the number of 'successful' calibrations
        totalCal = 0         # Store the number of 'attempted' calibrations
        
        deviation = None    # Note if this channel is deviating from the DB
        default = None      # Note if this channel has a default calibration constant

        # Store modified events in newevents for later
        newevents = set()

        # Loop over all events...
        for event in region.GetEvents():
            # Will this event be considered for the calibration?
            calibratableEvent = False

            totalCal += 1

            # The progress variable keeps track of how deep into the nested 'if'
            # statements we get for this event.
            progress = 0
            
            if event.runType == 'CIS':
                if event.data.has_key('problems'):
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

                            if not problems['No Response']:
                                progress = 5
                                self.nCalNoResp += 1

                                calibs.append(event.data['calibration'])
                                goodCal += 1
                                calibratableEvent = True

                                # We want to know if "problems['DB Deviation']" is ever
                                # not true.  The deviation 'variable' is set to 'None'
                                # as a default, in which case we set it with the current
                                # event's value.  If 'deviation' is 'True', since we are
                                # looking for cases where it is false, we will keep resetting
                                # it until it is false.
                                if problems.has_key('DB Deviation'):
                                    if deviation == None:
                                        deviation = problems['DB Deviation']
                                    if deviation == True:
                                        deviation = problems['DB Deviation']
                                else:
                                    print "CISRecalibrateProcedure: Couldn't find variable DB Deviation"

                                # We want to know when "problems['Default Calibration']" is
                                # ever not true.  The logic is the same as above.
                                if problems.has_key('Default Calibration'):
                                    if default == None:
                                        default = problems['Default Calibration']
                                    if default == True:
                                        default = problems['Default Calibration']
                                else:
                                    print "CISRecalibrateProcedure: Couldn't find variable default calibration"

                        else:
                            #event.data['dumpScan'] = True
                            pass # fail chi2 low
                    else:
                        #event.data['dumpScan'] = True
                        pass # fail rms
                else:
                    #event.data['dumpScan'] = True
                    pass # fail digital errors

            event.data['calibratableEvent'] = calibratableEvent
            newevents.add(event)

        calibratableRegion = False
        moreInfo = True
        sprogress = 0
        mean = 0
        if totalCal != 0:
            self.nChanTotal += 1
            sprogress = 1
        
            if goodCal != 0:
                self.nChanPassQFlag += 1
                sprogress = 2

                if default != False or deviation != False or self.all != False:
                    sprogress = 3
                    if default:
                        self.nChanDef += 1
                    elif deviation:
                        self.nChanDev += 1
                    else:
                        self.nChanAll += 1
            
                    if goodCal >= self.requiredCalibs:
                        sprogress = 4
                        self.nChanStatistics += 1

                        mean = ROOT.TMath.Mean(len(calibs), array('f', calibs))
                        rms = ROOT.TMath.RMS(len(calibs), array('f', calibs))
                        if 'low' in region.GetHash():
                            gain_theory = 1.27874994278
                            self.hlo.Fill(rms/gain_theory*100)
                        else:
                            gain_theory = 81.8399963379
                            self.hhi.Fill(rms/gain_theory*100)
                        
                        if rms/gain_theory*100 < 0.389059:
                            sprogress = 5
                            self.nChanStable += 1
                            calibratableRegion = True
                            moreInfo = False
                            mean = mean
        
        newevents2 = set()
        for event in newevents:
            event.data['calibratableRegion'] = calibratableRegion
            if not event.data.has_key('moreInfo'):
                event.data['moreInfo'] = moreInfo
            else:
                event.data['moreInfo'] = event.data['moreInfo'] | moreInfo    
            event.data['recalib_progress'] = sprogress
            if calibratableRegion:
                event.data['mean'] = mean
                
            newevents2.add(event)

        region.SetEvents(newevents2)

        
        
