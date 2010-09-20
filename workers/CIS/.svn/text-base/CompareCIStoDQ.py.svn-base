# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
import src.MakeCanvas

class CompareCIStoDQ(GenericWorker):
    "Compare CIS to DQ"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self, runType = 'CIS'):
        self.runType = runType
        self.nFailCisOnly = 0
        self.nFailDQOnly = 0
        self.nFailBoth = 0

    def ProcessStart(self):
        self.c1.Print("plots/compare_cis_dq.ps[")

    def ProcessStop(self):
        self.c1.Print("plots/compare_cis_dq.ps]")
        
        print 'Fail CIS Only: ', self.nFailCisOnly
        print 'Fail DQ Only: ', self.nFailDQOnly
        print 'Fail Both: ', self.nFailBoth
    
    def ProcessRegion(self, region):
        if 'gain' not in region.GetHash():
            return

        failcis = False
        faildq = False

#        scan = None

        for event in region.events:
            if event.runType == 'CIS' and event.data.has_key('isCalibrated'):
                failcis = not event.data['isCalibrated']

#            if event.runType == 'CIS' and event.data.has_key('isBad'):
#                if not event.data['isBad'] and event.data.has_key('scan'):
#                    scan = event.data['scan']
            if event.runType == 'DQ' and event.data.has_key('isBad'):
                faildq = event.data['isBad']

        x,y,z,w = region.GetNumber()
        y -= 1
        if failcis and faildq:
            self.nFailBoth += 1
            #print 'DQandCIS', region.GetHash(), region.GetHash(True)
            print './PyCisInfoADC.py -s -c 102009', x,y,z,w, '# both'
        elif faildq:
            self.nFailDQOnly += 1
            #print 'DQ      ', region.GetHash(), region.GetHash(True
            print './PyCisInfoADC.py -s -c 102009', x,y,z,w, '# dq'
        elif failcis:
            self.nFailCisOnly += 1
            #print '     CIS', region.GetHash(), region.GetHash(True)
            print './PyCisInfoADC.py -s -c 102009', x,y,z,w, '# cis'

        if failcis:
            x, y, z, w = region.GetNumber()
            h = region.GetHash()
            hs = h.split('_')
            print '%s\t%d\t%d\t%s' % (hs[1], y, z, hs[4]), '\tProbCIS', '\t%s'%region.GetHash(True)

        """
        if failcis or faildq:
            if not scan:
                return

            scan.Draw("ALP")
            pt = ROOT.TPaveText(0.8, 0.8, 1, 1)
            if failcis:
                pt.AddText('BAD-CIS')
            if faildq:
                pt.AddText('BAD-DQ')

            pt.Draw()

            self.c1.Print('plots/compare_cis_dq.ps')
            self.c1.Print('blah.C')
        """
        

