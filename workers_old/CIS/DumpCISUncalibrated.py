# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import os, datetime

class DumpCISUncalibrated(GenericWorker):
    "Dump uncalibrated regions"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self, runType = 'CIS'):
        self.runType = runType
        self.nFail = 0

        self.dir = getPlotDirectory()

        createDir('%s/pmt_numbering/fail_db_only' % self.dir)
        createDir('%s/pmt_numbering/fail_qflag_only' % self.dir)
        createDir('%s/pmt_numbering/fail_both' % self.dir)
        
        createDir('%s/fail_db_only' % self.dir)
        createDir('%s/fail_qflag_only' % self.dir)
        createDir('%s/fail_both' % self.dir)

    def ProcessStart(self):
        self.f_pmt = open('%s/pmt_numbering/stats' % self.dir, 'w')
        self.f_chan = open('%s/stats' % self.dir, 'w')

    def ProcessStop(self):
        print 'Number of Uncalibrated: ', self.nFail
        self.f_pmt.close()
        self.f_chan.close()
    
    def ProcessRegion(self, region):
        # Only look at each gain within some channel
        if 'gain' not in region.GetHash() or\
               region.GetEvents() == set():
            return

        foundCalibrated = False
        calibrated = False

        foundRepair = False
        repaired = False

        foundStable = False
        stable = False

        for event in region.GetEvents():
            if event.runType == self.runType and event.data.has_key('isCalibrated'):
                if event.data['isCalibrated']:
                    calibrated = True
                foundCalibrated = True

            if event.runType == 'Repair' and event.data.has_key('isBad'):
                if event.data['isBad']:
                    repaired = True
                foundRepair = True

            if event.runType == self.runType and event.data.has_key('isStable'):
                if event.data['isStable']:
                    stable = True
                foundStable = True

        
        if not foundCalibrated or calibrated:
            return

        self.nFail += 1
        
        self.c1.cd()

        dirstr = ''
        if event.data['isCalibrated_db']:
            dirstr = 'fail_qflag_only'
            self.f_chan.write('%s bad qflags over last month\n' % region.GetHash())
            self.f_pmt.write('%s bad qflags over last month\n' % region.GetHash(1))
        elif event.data['isCalibrated_qflag']:
            dirstr = 'fail_db_only'
            self.f_chan.write('%s db variation more than 1 percent over last month\n' % region.GetHash())
            self.f_pmt.write('%s db variation more than 1 percent over last month\n' % region.GetHash(1))
        else:
            dirstr = 'fail_both'
            self.f_chan.write('%s db variation more than 1 percent and bad qflags over last month\n' % region.GetHash())
            self.f_pmt.write('%s db variation more than 1 percent and bad qflags over last month\n' % region.GetHash(1))

        self.c1.Print("%s/%s/uncalib_%s_%s.ps[" % (self.dir, dirstr, self.runType,region.GetHash()))

        for event in region.GetEvents():
            if event.data.has_key('scan'):
                scan = event.data['scan']
                scan.GetYaxis().SetRangeUser(0.0, 1000.0)
                scan.Draw("ALP")

                tl = ROOT.TLatex()
                tl.SetTextAlign(12)
                tl.SetTextSize(0.04)
                tl.SetNDC()

                cstr = region.GetHash()[8:]
                pstr = region.GetHash(1)[16:19]
                csplit = cstr.split('_')
                newstr = csplit[0] + csplit[1][1:] + ' ' + csplit[2] + '/' + pstr + ' ' + csplit[3]
                
                xoffset = 0.15
                tl.DrawLatex(xoffset,0.95, newstr)
                tl.DrawLatex(xoffset,0.85,"Run %d" % event.runNumber)
                tl.DrawLatex(xoffset,0.80,"Slope This Run: %f" % event.data['calibration'])
                tl.DrawLatex(xoffset,0.75,"DB Slope: %f" % event.data['calibration_db'])
                
                tl.DrawLatex(xoffset,0.70,"DB Variation: %f %s" % (100*(event.data['calibration'] - event.data['calibration_db'])/event.data['calibration_db'], '%'))

                if abs(event.data['deviation']) < 0.01:
                    tl.DrawLatex(xoffset, 0.65,"Pass 1% Flag")
                else:
                    tl.DrawLatex(xoffset, 0.65,"Fail 1% Flag")

                if event.data['quality_flag'] & 16 == 16:
                    tl.DrawLatex(xoffset, 0.60,"Pass Max Point Flag")
                else:
                    tl.DrawLatex(xoffset, 0.60,"Fail Max Point Flag")

                if event.data['quality_flag'] & 8 == 8:
                    tl.DrawLatex(xoffset, 0.55,"Pass Likely Calib Flag")
                else:
                    tl.DrawLatex(xoffset, 0.55,"Fail Likely Calib Flag")

#                tl.DrawLatex(xoffset, 0.50,"nDigitalErrors: %d" % int(event.data['nDigitalErrors']))

                self.c1.Print("%s/%s/uncalib_%s_%s.ps" % (self.dir, dirstr, self.runType,region.GetHash()))

                                                                                                

        self.c1.Print("%s/%s/uncalib_%s_%s.ps]" % (self.dir, dirstr, self.runType,region.GetHash()))
        if not os.path.exists("%s/pmt_numbering/%s/uncalib_%s_%s.ps" % (self.dir, dirstr, self.runType,region.GetHash(1))):
            os.symlink("../../%s/uncalib_%s_%s.ps" % (dirstr, self.runType,region.GetHash()),\
                       "%s/pmt_numbering/%s/uncalib_%s_%s.ps" % (self.dir, dirstr, self.runType,region.GetHash(1)))
        
        
        
