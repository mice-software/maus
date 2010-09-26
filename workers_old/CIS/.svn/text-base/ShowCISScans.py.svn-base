# Author: Christopher Tunnell <tunnell@hep.uchicago.edu>
#
# March 04, 2009
#

from src.GenericWorker import *
from src.oscalls import *
import src.MakeCanvas
import os, datetime
import pickle

class ShowCISScans(GenericWorker):
    "Create postscript files with the CIS scans and diagnostic information"

    c1 = src.MakeCanvas.MakeCanvas()

    def __init__(self, runType = 'CIS', all=False):
        self.runType = runType
        self.all = all
        self.nFail = 0

        self.dir = getPlotDirectory() + '/cis'

        createDir('%s/pmt_numbering/scans_fail_db_only' % self.dir)
        createDir('%s/pmt_numbering/scans_fail_qflag_only' % self.dir)
        createDir('%s/pmt_numbering/scans_fail_both' % self.dir)
        createDir('%s/pmt_numbering/scans_pass' % self.dir)

        createDir("%s/rms" % self.dir)
        
        createDir('%s/scans_fail_db_only' % self.dir)
        createDir('%s/scans_fail_qflag_only' % self.dir)
        createDir('%s/scans_fail_both' % self.dir)
        createDir('%s/scans_pass' % self.dir)

        if os.path.exists('%s/pmt_numbering/stats' % self.dir):
            os.unlink('%s/pmt_numbering/stats' % self.dir)
            
        if os.path.exists('%s/stats' % self.dir):
            os.unlink('%s/stats' % self.dir)
                        

    def ProcessStart(self):
        self.f_pmt = open('%s/pmt_numbering/stats' % self.dir, 'a')
        self.f_chan = open('%s/stats' % self.dir, 'a')
        

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

        sevent = None # selected event; most recent run's good event

        for event in region.GetEvents():
            criteria = 'moreInfo'
            if event.runType == self.runType and event.data.has_key(criteria):
                if not event.data[criteria]:
                    calibrated = True
                foundCalibrated = True

                if sevent == None or sevent.runNumber < event.runNumber:
                    sevent = event

        if not foundCalibrated or (calibrated and not self.all):
            return

        self.nFail += 1
        
        self.c1.cd()

        dirstr = ''
        
        assert(sevent.data.has_key('problems'))
        
        problems = sevent.data['problems']

        assert(problems.has_key('DB Deviation'))
        assert(event.data.has_key('goodRegion'))
        
        if sevent.data['goodRegion']:
            dirstr = 'scans_pass'
        elif problems['DB Deviation'] and sevent.data['flag_progress'] >= 6:
            dirstr = 'scans_fail_db_only'
            self.f_chan.write('%s db variation more than 1 percent over last month\n' % region.GetHash())
            self.f_pmt.write('%s db variation more than 1 percent over last month\n' % region.GetHash(1))
        elif not problems['DB Deviation'] and sevent.data['flag_progress'] < 6:
            dirstr = 'scans_fail_qflag_only'
            self.f_chan.write('%s bad qflags over last month\n' % region.GetHash())
            self.f_pmt.write('%s bad qflags over last month\n' % region.GetHash(1))
        else:
            dirstr = 'scans_fail_both'
            self.f_chan.write('%s db variation more than 1 percent and bad qflags over last month\n' % region.GetHash())
            self.f_pmt.write('%s db variation more than 1 percent and bad qflags over last month\n' % region.GetHash(1))

        self.c1.Print("%s/%s/uncalib_%s_%s.ps[" % (self.dir, dirstr, self.runType,region.GetHash()))

        for event in region.GetEvents():
            if event.data.has_key('scan'):
                self.c1.Divide(1, 1)
                self.c1.Modified()
                
                scan = event.data['scan']
                scan.GetYaxis().SetRangeUser(0.0, 1000.0)
                scan.Draw("ALP")

                self.c1.Print("%s/%s/uncalib_%s_%s.eps" % (self.dir, dirstr, self.runType,region.GetHash()))

                tl = ROOT.TLatex()
                tl.SetTextAlign(12)
                tl.SetTextSize(0.03)
                tl.SetNDC()

                cstr = region.GetHash()[8:]
                pstr = region.GetHash(1)[16:19]
                csplit = cstr.split('_')
                newstr = csplit[0] + csplit[1][1:] + ' ' + csplit[2] + '/' + pstr + ' ' + csplit[3]
                
                xoffset = 0.20

                tl.DrawLatex(xoffset,0.98, newstr)

                dy = 0.04
                location = 0.9

                tl.DrawLatex(xoffset,location,"Run %d" % event.runNumber)
                location -= dy

                tl.DrawLatex(xoffset,location,"Slope This Run: %f" % event.data['calibration'])
                location -= dy

                tl.DrawLatex(xoffset,location,"DB Slope: %f" % event.data['calibration_db'])
                location -= dy
                
                tl.DrawLatex(xoffset,location,"DB Variation: %f %s" % (100*(event.data['calibration'] - event.data['calibration_db'])/event.data['calibration_db'], '%'))
                location -= dy

                for problem, value in event.data['problems'].iteritems():
                    tl.DrawLatex(xoffset, location,"%s: %s" % (problem, str(value)))
                    location -= dy

                self.c1.Print("%s/%s/uncalib_%s_%s.ps" % (self.dir, dirstr, self.runType,region.GetHash()))

                if event.data.has_key('injections'):
                    self.c1.Clear()
                    if 'lowgain' in region.GetHash():
                        self.c1.Divide(4,5)
                    else:
                        self.c1.Divide(3,4)
                    self.c1.Modified()
                    i_canvas = 1
                    tl.SetTextSize(0.08)
                    number = region.GetNumber(1)
                    
                    inj_hists = event.data['injections']
                    for x, y in inj_hists.iteritems():
                        if 'lowgain' in region.GetHash():
                            pass#self.c1.cd(i_canvas+4)
                        else:
                            pass#self.c1.cd(i_canvas+3)
                        i_canvas += 1

                        y.GetXaxis().SetTitle('ADC counts')
                        #y.SetLineWidth(6)
                        y.Draw()
                        tl.DrawLatex(xoffset,0.95, "%0.2f pC, RMS %0.2f" % (x, y.GetRMS()))
                        self.c1.Print("%s/rms/rms_%0.2f_%0.2f.ps" % (self.dir, x, y.GetRMS()))
                        #tl.DrawLatex(xoffset,0.85, "RMS %f" % y.GetRMS())
                        

                    self.c1.cd()
                    tl.SetTextSize(0.03)
                    tl.DrawLatex(0.10, 0.97, "Fixed-charge Distribution run %d %s" % (event.runNumber, region.GetHash()[8:]))

                    self.c1.Print("%s/%s/uncalib_%s_%s.ps" % (self.dir, dirstr, self.runType,region.GetHash()))
                    self.c1.Divide(1, 1,)
                    self.c1.Modified()

                if event.data.has_key('samples'):
                    #a = ROOT.gStyle.GetPadBorderSize()
                    #b = ROOT.gStyle.GetFrameBorderSize()
                    ROOT.gStyle.SetPadBorderSize(0)
                    ROOT.gStyle.SetFrameBorderSize(0)
                    ROOT.gStyle.SetOptStat(0)

                    hists = event.data['samples']

                    x_dac = 16
                    y_phase = 16
                    self.c1.Divide(x_dac, y_phase)
                    for i in range(event.data['maxphase']):
                        for j in range(event.data['maxdac']):
                            if len(hists[i][j]) != 4:
                                print len(hists[i][j]) , 'ouch', i, j

                            self.c1.cd((i+1)*x_dac + (j+1) + 1)
                            hists[i][j][0].GetXaxis()
                            hists[i][j][0].SetMinimum(0)
                            hists[i][j][0].SetMaximum(1023)
                            hists[i][j][0].Draw('')
                            hists[i][j][1].SetLineColor(ROOT.kRed)
                            hists[i][j][1].Draw('SAME')
                            hists[i][j][2].SetLineColor(ROOT.kBlue)
                            hists[i][j][2].Draw('SAME')
                            hists[i][j][3].SetLineColor(ROOT.kGreen)
                            hists[i][j][3].Draw('SAME')
                            
                            
                    self.c1.Modified()
                    self.c1.cd(0)
                    
                    tl1 = ROOT.TLatex(0.10, 0.03, "Injected Charge")
                    tl2 = ROOT.TLatex(0.03, 0.10, "Phase")
                    tl3 = ROOT.TLatex(0.10, 0.97, "Fit-Range Samples run %d %s" % (event.runNumber, region.GetHash()[8:]))
                    
                    tl2.SetTextAngle(90)
                    for tl in [tl1, tl2, tl3]:
                        tl.SetTextAlign(12)
                        tl.SetTextSize(0.06)
                        tl.SetNDC()
                        tl.Draw()
                        
                    self.c1.Print("%s/%s/uncalib_%s_%s.ps" % (self.dir, dirstr, self.runType,region.GetHash()))
                    
                    self.c1.Clear()
                    self.c1.Divide(1, 1)
                    self.c1.Modified()
                    #ROOT.gStyle.SetPadBorderSize(a)
                    #ROOT.gStyle.SetFrameBorderSize(b)

        

        self.c1.Print("%s/%s/uncalib_%s_%s.ps]" % (self.dir, dirstr, self.runType,region.GetHash()))
        if os.path.exists("%s/pmt_numbering/%s/uncalib_%s_%s.ps" % (self.dir, dirstr, self.runType,region.GetHash(1))):
            os.unlink("%s/pmt_numbering/%s/uncalib_%s_%s.ps" % (self.dir, dirstr, self.runType,region.GetHash(1)))
            
        os.symlink("../../%s/uncalib_%s_%s.ps" % (dirstr, self.runType,region.GetHash()),\
                   "%s/pmt_numbering/%s/uncalib_%s_%s.ps" % (self.dir, dirstr, self.runType,region.GetHash(1)))


        if sevent.data.has_key('goodRegion') and sevent.data.has_key('calibratableRegion'):
            if sevent.data['goodRegion'] and sevent.data['calibratableRegion']:
                gcstr = "Pg_Pc"
            elif sevent.data['goodRegion'] and not sevent.data['calibratableRegion']:
                gcstr = "Pg_Fc"
            elif not sevent.data['goodRegion'] and sevent.data['calibratableRegion']:
                gcstr = "Fg_Pc"
            elif not sevent.data['goodRegion'] and not sevent.data['calibratableRegion']:
                gcstr = "Fg_Fc"
                                
            createDir('%s/%s' % (self.dir, gcstr))

            if os.path.exists("%s/%s/uncalib_%s_%s.ps" % (self.dir, gcstr, self.runType,region.GetHash(0))):
                os.unlink("%s/%s/uncalib_%s_%s.ps" % (self.dir, gcstr, self.runType,region.GetHash(0)))

            os.symlink("../%s/uncalib_%s_%s.ps" % (dirstr, self.runType,region.GetHash()),\
                       "%s/%s/uncalib_%s_%s.ps" % (self.dir, gcstr, self.runType,region.GetHash(0)))
            
        if sevent.data.has_key('recalib_progress'):
            createDir('%s/recalib%d' % (self.dir, sevent.data['recalib_progress']))
            if os.path.exists("%s/recalib%d/uncalib_%s_%s.ps" % (self.dir, sevent.data['recalib_progress'], self.runType,region.GetHash(0))):
                os.unlink("%s/recalib%d/uncalib_%s_%s.ps" % (self.dir, sevent.data['recalib_progress'], self.runType,region.GetHash(0)))
                            
            os.symlink("../%s/uncalib_%s_%s.ps" % (dirstr, self.runType,region.GetHash()),\
                       "%s/recalib%d/uncalib_%s_%s.ps" % (self.dir, sevent.data['recalib_progress'], self.runType,region.GetHash(0)))
            
                      
        if sevent.data.has_key('flag_progress'):
            createDir('%s/flag%d' % (self.dir, sevent.data['flag_progress']))
            if os.path.exists("%s/flag%d/uncalib_%s_%s.ps" % (self.dir, sevent.data['flag_progress'], self.runType,region.GetHash(0))):
                os.unlink("%s/flag%d/uncalib_%s_%s.ps" % (self.dir, sevent.data['flag_progress'], self.runType,region.GetHash(0)))
                
            os.symlink("../%s/uncalib_%s_%s.ps" % (dirstr, self.runType,region.GetHash()),\
                       "%s/flag%d/uncalib_%s_%s.ps" % (self.dir, sevent.data['flag_progress'], self.runType,region.GetHash(0)))

        if sevent.data.has_key('comp_chi_rms'):
            [pass_chirms, pass_qflag] = event.data['comp_chi_rms']
            name = 'compare_%s_chirms_%s_qflag' % (str(pass_chirms), str(pass_qflag))
            
            createDir('%s/%s' % (self.dir, name))
            if os.path.exists("%s/%s/uncalib_%s_%s.ps" % (self.dir, name, self.runType,region.GetHash(0))):
                os.unlink("%s/%s/uncalib_%s_%s.ps" % (self.dir, name, self.runType,region.GetHash(0)))
                
            os.symlink("../%s/uncalib_%s_%s.ps" % (dirstr, self.runType,region.GetHash()),\
                       "%s/%s/uncalib_%s_%s.ps" % (self.dir, name, self.runType,region.GetHash(0)))
                                                            
                                                            
        
        
        
        
        
