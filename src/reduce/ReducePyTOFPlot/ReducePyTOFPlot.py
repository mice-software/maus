"""
ReducePyTOFPlot fills TOF histograms for slab hits and space points,
draws them, refreshes the canvases and prints to eps at the end of
run.
"""
#  This file is part of MAUS: http://micewww.pp.rl.ac.uk:8080/projects/maus
# 
#  MAUS is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  MAUS is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
# 
#  You should have received a copy of the GNU General Public License
#  along with MAUS.  If not, see <http://www.gnu.org/licenses/>.
#
# Turn off false positives related to ROOT
#pylint: disable = E1101
# Disable messages about too many branches and too many lines.
#pylint: disable = R0912
#pylint: disable = R0915
import ROOT
from ReducePyROOTHistogram import ReducePyROOTHistogram

class ReducePyTOFPlot(ReducePyROOTHistogram): # pylint: disable=R0902
    """
    ReducePyTOFPlots plots several TOF histograms for each spill.
    Currently the following histograms are filled:
    - x,y slab_hits from spill.
    - For each slab hit, fill PMT(s) that is(are) hit.
    - Number of reconstructed space points per particle event.
    - 1d x and y of space points.
    - 2d y vs x of space points.
    
    Histograms are drawn on different canvases.
    The canvases are refreshed every N spills where N = refresh_rate
    set via refresh_rate data card value (see below).
   
    The default is to run ROOT in batch mode
    To run in interactive mode, set root_batch_mode = 0 in the data card
    (see below). 
    
    At the end of the run, the canvases are printed to eps files

    A sequence of 13 histograms are output as JSON documents of form:

    @verbatim
    {"image": {"keywords": [...list of image keywords...],
               "description":"...a description of the image...",
               "tag": TAG,
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}}
    @endverbatim

    These are as follows and have the following TAGs:

    - "tof_hit_x" - TOF raw hits X.
    - "tof_hit_y" - TOF raw hits Y.
    - "tof_pmt00" - TOF PMT Plane 0 PMT 0.
    - "tof_pmt01"  - TOF PMT Plane 0 PMT 1.
    - "tof_pmt10"  - TOF PMT Plane 1 PMT 0.
    - "tof_pmt11"  - TOF PMT Plane 1 PMT 1.
    - "tof_nsp" - TOF number of space points.
    - "tof_sp_x" - TOF space points X.
    - "tof_sp_y" - TOF space points Y.
    - "tof_xy_0" - TOF space points 2D.
    - "tof_xy_1" - TOF space points 2D.
    - "tof_xy_2" - TOF space points 2D.
    - "tof_time_01" - TOF01 time.
    - "tof_time_12" - TOF12 time.
    - "tof_time_02" - TOF02 time.
    - "tof0_nsp_spill" - TOF0 spacepoints per spill
    - "tof1_nsp_spill" - TOF1 spacepoints per spill
    - "tof2_nsp_spill" - TOF2 spacepoints per spill

    If "histogram_auto_number" (see below) is "true" then the TAG will
    have a number N appended where N means that the histogram was
    produced as a consequence of the (N + 1)th spill processed  by the
    worker. The number will be zero-padded to form a six digit string
    e.g. "00000N". If "histogram_auto_number" is false then no such
    number is appended 

    In cases where a spill is input that contains errors (e.g. is
    badly formatted or is missing the data needed to update a
    histogram) then a spill is output which is just the input spill
    with an "errors" field containing the error e.g.

    @verbatim
    {"errors": {..., "bad_json_document": "unable to do json.loads on input"}}
    {"errors": {..., "no_space_points": "no space points"}}
    @endverbatim

    The caller can configure the worker and specify:

    -Image type ("histogram_image_type"). Must be one of those
     supported by ROOT (currently "ps", "eps", "gif", "jpg", "jpeg",
     "pdf", "svg", "png"). Default "eps".
    -Auto-number ("histogram_auto_number"). Default: false. Flag
     that determines if the image tag (see above) has the spill count
     appended to it or not.
    -ROOT batch mode ("root_batch_mode"). Default: 0 (false). Flag
     indicating whether ROOT should be run in batch or interactive 
     mode.
    -Refresh rate ("refresh_rate"). Default: 5. Number of spills
     input before the next set of histograms are output.
    """

    def __init__(self):
        """
        Set initial attribute values.
        @param self Object reference.
        """
        ReducePyROOTHistogram.__init__(self)
        # Do initialisation specific to this class.
        # Refresh_rate determines how often (in spill counts) the
        # canvases are updated.
        self.refresh_rate = 5
        # Histogram initializations. they are defined explicitly in
        # init_histos.
        self.legend = None
        self._ht01 = None
        self._ht02 = None
        self._ht12 = None
        self.hpmthits = None
        self.hspslabx_0 = None
        self.hspslabx_1 = None
        self.hspslabx_2 = None
        self.hspslaby_0 = None
        self.hspslaby_1 = None
        self.hspslaby_2 = None
        self.hspxy = None
        self.hnsp_0 = None
        self.hnsp_1 = None
        self.hnsp_2 = None
        self.hnsp_spill = None
        self.hnsp_vs_spill = None
        self.hslabhits = None

        self.canvas_tof = None
        self.canvas_pmt = None
        self.canvas_hits_x = None
        self.canvas_hits_y = None
        self.canvas_nsp = None
        self.canvas_nsp_spill = None
        self.canvas_nsp_vs_spill = None
        self.canvas_sp_x = None
        self.canvas_sp_y = None
        self.canvas_sp_xy = None
        # Has an end_of_run been processed?
        self.run_ended = False
        self.spillnum = 0
        self.have_sp = False

    def _configure_at_birth(self, config_doc):
        """
        Configure worker from data cards. Overrides super-class method. 
        @param self Object reference.
        @param config_doc JSON document.
        @returns True if configuration succeeded.
        """
        # Read in configuration flags and parameters - these will
        # overwrite whatever defaults were set in __init__.
        if 'reduce_plot_refresh_rate' in config_doc:
            self.refresh_rate = int(config_doc["reduce_plot_refresh_rate"])
        # Initialize histograms, setup root canvases, and set root
        # styles.
        self.__init_histos()
        self.run_ended = False
        return True


    def _update_histograms(self, spill):
        """
        Process a spill, get slab hits via get_slab_hits get space
        points via get_space_points, update the histograms then
        creates JSON documents in the format described above. 
        @param self Object reference.
        @param spill Current spill.
        @returns list of JSON documents. If the the spill count is
        divisible by the current refresh rate then a list of 3
        histogram JSON documents are returned. Otherwise a single list
        with the input spill is returned. 
        @throws ValueError if "slab_hits" and "space_points" information
        is missing from the spill.
        """
        daq_evtype = spill.GetSpill().GetDaqEventType()
        if daq_evtype == "end_of_run":
            if (not self.run_ended):
                self.update_histos()
                self.run_ended = True
                return self.get_histogram_images()
            else:
                return []

        # do not try to get data from start/end spill markers
        data_spill = True
        if daq_evtype == "start_of_run" \
              or daq_evtype == "start_of_burst" \
              or daq_evtype == "end_of_burst":
            data_spill = False

        # Get TOF slab hits & fill the relevant histograms.
        if data_spill and not self.get_slab_hits(spill): 
            raise ValueError("slab_hits not in spill")

        # Get TOF space points & fill histograms.
        if data_spill and not self.get_space_points(spill):
            # raise ValueError("space_points not in spill")
            self.have_sp = False
            print __name__, ': ', 'No space_points in spill'

        # Refresh canvases at requested frequency.
        if self.spill_count % self.refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return []

    def get_slab_hits(self, spill):
        """ 
        Get the TOF slab hits and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "slab_hits" in
        the spill.
        """
        if spill.GetSpill().GetReconEventSize() == 0:
            raise ValueError("recon_events not in spill")
        # print 'nevt = ', len(spill['recon_events']), spill['recon_events']
        reconevents = spill.GetSpill().GetReconEvents()
        # print '# recon events = ',reconevents[0].GetPartEventNumber()
        for evn in range(spill.GetSpill().GetReconEventSize()):
            tof_event = reconevents[evn].GetTOFEvent()
            if tof_event is None:
                # print 'no tof event'
                raise ValueError("tof_event not in recon_events")
            # Return if we cannot find slab_hits in the event.
            tof_slab_hits = tof_event.GetTOFEventSlabHit()
            #if 'tof_slab_hits' not in spill['recon_events'][evn]['tof_event']:
            if tof_slab_hits is None:
                return False

            tof0_sh = tof_slab_hits.GetTOF0SlabHitArray()
            tof1_sh = tof_slab_hits.GetTOF1SlabHitArray()
            tof2_sh = tof_slab_hits.GetTOF2SlabHitArray()
            sh_list = [tof0_sh, tof1_sh, tof2_sh ]
            for index, dethits in enumerate(sh_list):
                # print 'index, size >> ',index, dethits.size()
                for i in range(dethits.size()):
                    pos = dethits[i].GetSlab()
                    # print '>> hit#, slab ',i,pos
                    plane_num = dethits[i].GetPlane()
                    # print '>> hit#, plane ',i,plane_num
                    if plane_num < 0 or plane_num > 1:
                        return False
                    self.hslabhits[index][plane_num].Fill(pos)
                    # plane 0, pmt0 hit for this slab
                    if (dethits[i].GetPmt0() is not None):
                        self.hpmthits[index][plane_num][0].Fill(pos)
                    # plane 0, pmt1 hit for this slab
                    if (dethits[i].GetPmt1() is not None):
                        self.hpmthits[index][plane_num][1].Fill(pos)
        return True

    def get_space_points(self, spill): # pylint: disable = R0914
        """ 
        Get the TOF space points and update the histograms.

        Go through the TOF0,1,2 space points
        Make sure it is not null
        Get the number of space points for each particle event
        For each space point, get the x & y
        Get the time & find the Time-of-Flight between 0->1, 0->2, 1->2,
        Fill histograms

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "space_points" in
        the spill.
        """
        self.spillnum = self.spillnum + 1

        nsp_spill = []
        nsp_spill = [0] * 3

        if spill.GetSpill().GetReconEventSize() == 0:
            raise ValueError("recon_events not in spill")
        reconevents = spill.GetSpill().GetReconEvents()
        # print '# recon events = ',reconevents[0].GetPartEventNumber()
        for evn in range(spill.GetSpill().GetReconEventSize()):
            tof_event = reconevents[evn].GetTOFEvent()
            if tof_event is None:
                raise ValueError("tof_event not in recon_events")
            # Return if we cannot find slab_hits in the event.
            tof_spoints = tof_event.GetTOFEventSpacePoint()
            if tof_spoints is None:
                return False
            sp_tof0 = tof_spoints.GetTOF0SpacePointArray()
            sp_tof1 = tof_spoints.GetTOF1SpacePointArray()
            sp_tof2 = tof_spoints.GetTOF2SpacePointArray()

            # TOF0
            if sp_tof0 is not None:
                self.hnsp_0.Fill(sp_tof0.size())
                nsp_spill[0] = nsp_spill[0] + sp_tof0.size()

                for i in range(sp_tof0.size()):
                    # print 'nsp0: ',i,sp_tof0[i]
                    spnt_x = sp_tof0[i].GetSlabx()
                    spnt_y = sp_tof0[i].GetSlaby()
                    self.hspxy[0].Fill(spnt_y, spnt_x)
                    self.hspslabx_0.Fill(spnt_x)
                    self.hspslaby_0.Fill(spnt_y)
                    if sp_tof1 is not None :
                        if sp_tof0.size() == 1 and sp_tof1.size() == 1:
                            t_0 = sp_tof0[i].GetTime()
                            t_1 = sp_tof1[i].GetTime()
            else:
                self.hnsp_0.Fill(0)           

            # TOF2
            if sp_tof2 is not None:
                self.hnsp_2.Fill(sp_tof2.size())
                nsp_spill[2] = nsp_spill[2] + sp_tof2.size()

                for i in range(sp_tof2.size()):
                    # print 'nsp0: ',i,sp_tof0[i]
                    spnt_x = sp_tof2[i].GetSlabx()
                    spnt_y = sp_tof2[i].GetSlaby()
                    self.hspxy[2].Fill(spnt_y, spnt_x)
                    self.hspslabx_2.Fill(spnt_x)
                    self.hspslaby_2.Fill(spnt_y)
                    if sp_tof1 is not None :
                        if sp_tof2.size() == 1 and sp_tof1.size() == 1:
                            t_2 = sp_tof2[i].GetTime()
                            t_1 = sp_tof1[i].GetTime()
                            self._ht12.Fill(t_2-t_1)
                    if sp_tof0 is not None :
                        if sp_tof2.size() == 1 and sp_tof0.size() == 1:
                            t_2 = sp_tof2[i].GetTime()
                            t_0 = sp_tof0[i].GetTime()
                            self._ht02.Fill(t_2-t_0)
            else:
                self.hnsp_2.Fill(0)           

            # TOF1
            if sp_tof1 is not None:
                self.hnsp_1.Fill(sp_tof1.size())
                nsp_spill[1] = nsp_spill[1] + sp_tof1.size()

                for i in range(sp_tof1.size()):
                    # print 'nsp0: ',i,sp_tof0[i]
                    spnt_x = sp_tof1[i].GetSlabx()
                    spnt_y = sp_tof1[i].GetSlaby()
                    self.hspxy[1].Fill(spnt_y, spnt_x)
                    self.hspslabx_1.Fill(spnt_x)
                    self.hspslaby_1.Fill(spnt_y)
                    if sp_tof1 is not None :
                        if sp_tof2.size() == 1 and sp_tof1.size() == 1:
                            t_2 = sp_tof2[i].GetTime()
                            t_1 = sp_tof1[i].GetTime()
                            self._ht12.Fill(t_2-t_1)
                    if sp_tof0 is not None :
                        if sp_tof1.size() == 1 and sp_tof0.size() == 1:
                            t_1 = sp_tof1[i].GetTime()
                            t_0 = sp_tof0[i].GetTime()
                            self._ht01.Fill(t_1-t_0)
            else:
                self.hnsp_2.Fill(0)           

        for j in range (3): 
            if nsp_spill[j] > 0:
                self.hnsp_spill[j].Fill(nsp_spill[j])
            self.hnsp_vs_spill[j].Fill(self.spillnum, nsp_spill[j])
        return True

    def __init_histos(self): #pylint: disable=R0201, R0914
        """
        Initialise ROOT to display histograms.

        @param self Object reference.
        """ 
        # have root run quietly without verbose informationals
        ROOT.gErrorIgnoreLevel = 4001

        # white canvas
        ROOT.gROOT.SetStyle("Plain")
        
        #turn off stat box
        ROOT.gStyle.SetOptStat(0)
        
        #sensible color palette
        ROOT.gStyle.SetPalette(1)
        
        # xy grid on canvas
        ROOT.gStyle.SetPadGridX(1)
        ROOT.gStyle.SetPadGridY(1)
        self.legend = ROOT.TLegend(0.6, 0.7, 0.89, 0.89)
        ROOT.SetOwnership( self.legend, 1 ) 
        ROOT.gStyle.SetFillColor(0)
        # define histograms
        self._ht01 = ROOT.TH1F("ht01", "TOF0->1;Time (ns);;", 200, 20, 40)
        self._ht12 = ROOT.TH1F("ht12", "TOF1->2;Time (ns);;", 200, 25, 45)
        self._ht02 = ROOT.TH1F("ht02", "TOF0->2;Time (ns);;", 300, 50, 80)
        
        
        self.hspslabx_0 = ROOT.TH1F("spx0", "SpacePoints X-plane;SlabX;",
                                     10, -0.5, 9.5)
        self.hspslaby_0 = ROOT.TH1F("spy0", "SpacePoints Y-plane;SlabY;",
                                     10, -0.5, 9.5)
        self.hspslabx_1 = ROOT.TH1F("spx1", "SpacePoints X-plane;SlabX;",
                                     10, -0.5, 9.5)
        self.hspslaby_1 = ROOT.TH1F("spy1", "SpacePoints Y-plane;SlabY;",
                                     10, -0.5, 9.5)
        self.hspslabx_1.SetLineColor(2)
        self.hspslaby_1.SetLineColor(2) 
        self.hspslabx_2 = ROOT.TH1F("spx2", "SpacePoints X-plane;SlabX;",
                                     10, -0.5, 9.5)
        self.hspslaby_2 = ROOT.TH1F("spy2", "SpacePoints Y-plane;SlabY;",
                                     10, -0.5, 9.5)
        self.hspslabx_2.SetLineColor(4)
        self.hspslaby_2.SetLineColor(4) 
        
        self.hnsp_0 = ROOT.TH1F("hnsp_0", 
                                "#Space Points;#space points in event;;",
                                 4, -0.5, 3.5)
        self.hnsp_0.SetLineColor(1)
        self.hnsp_1 = ROOT.TH1F("hnsp_1", 
                                "#Space Points;#space points in event;;",
                                 4, -0.5, 3.5)
        self.hnsp_1.SetLineColor(2)
        #self.hnsp_1.SetFillStyle(4000)
        self.hnsp_2 = ROOT.TH1F("hnsp_2", 
                                 "#Space Points;#space points in event;;",
                                 4, -0.5, 3.5)
        self.hnsp_2.SetLineColor(4)

        self.hnsp_spill = []
        self.hnsp_vs_spill = []
        for i in range (0, 3):
            hcolor = pow(2, i)
            histname = "hnsp_vs_spill_tof%d" % (i)
            title = "TOF%d Space Points in Spill;Spill;#Space Points" % (i)
            nbins = 5000
            xlo = 1.0
            xhi = 0.0
            self.hnsp_vs_spill.append(ROOT.TH1F(histname,
                                              title,
                                              nbins, xlo, xhi))
            self.hnsp_vs_spill[i].SetBit(ROOT.TH1.kCanRebin)

            histname = "hnsp_spill_tof%d" % (i)
            title = "#Space Points in Spill;#space points per spill;;"
            nbins = 60
            xlo = -0.5
            xhi = 59.5
            self.hnsp_spill.append(ROOT.TH1F(histname,
                                              title,
                                              nbins, xlo, xhi))
            self.hnsp_spill[i].SetLineColor(hcolor)

        self.hslabhits = [[]]
        for i in range (0, 3):
            self.hslabhits.append([])
            for j in range (0, 2):
                histname = "hslabhits%d%d" % (i, j)
                if (j == 0): 
                    title = "Slab Hits X-plane; SlabX;"
                if (j == 1): 
                    title = "Slab Hits Y-plane;SlabY;"
                nbins = 10
                xlo = -0.5
                xhi = 9.5
                self.hslabhits[i].append(ROOT.TH1F(histname,
                                                   title,
                                                   nbins, xlo, xhi))
                if i == 0:
                    self.hslabhits[i][j].SetLineColor(i+1)
                else:
                    self.hslabhits[i][j].SetLineColor(2*i)

        self.hpmthits = [[[]]]
        for i in range (0, 3):
            self.hpmthits.append([])
            for j in range (0, 2):
                self.hpmthits[i].append([])
                for k in range (0, 2):
                    histname = "hpmthits%d%d%d" % (i, j, k)
                    title = "plane %d, pmt %d; Slab;" % (j, k)
                    nbins = 10
                    xlo = -0.5
                    xhi = 9.5
                    self.hpmthits[i][j].append(ROOT.TH1F(histname, 
                                                         title,
                                                         nbins, xlo, xhi))
                    if i == 0:
                        self.hpmthits[i][j][k].SetLineColor(i+1)
                    else:
                        self.hpmthits[i][j][k].SetLineColor(2*i)

        # Create canvases
        #
        # Draw() of histos has to be done only once
        # for updating the histograms, just Modified() and Update() on canvases
        # the update/refresh is done in update_histos()
        
        # tof times between 0-1-2
        self.canvas_tof = []
        for i in range (3):
            cname = "tof%d" % (i)
            self.canvas_tof.append(ROOT.TCanvas(cname))
        self.canvas_tof[0].cd()
        self._ht01.Draw()
        self.canvas_tof[1].cd()
        self._ht12.Draw()
        self.canvas_tof[2].cd()
        self._ht02.Draw()

        
        # Slab Hits x
        canvas_name = "hits_x"
        self.canvas_hits_x = ROOT.TCanvas(canvas_name, canvas_name, 800, 800)
        self.canvas_hits_x.cd()
        self.hslabhits[1][0].Draw()
        for i in range (0, 3):
            self.hslabhits[i][0].Draw("same")

        # Slab Hits y
        self.canvas_hits_y = ROOT.TCanvas("hits_y", "hits_y", 800, 800)
        self.canvas_hits_y.cd()
        self.hslabhits[1][1].Draw()
        for i in range (0, 3):
            self.hslabhits[i][1].Draw("same")

        # Number of Space points
        self.canvas_nsp = ROOT.TCanvas("nsp", "nsp", 800, 800)
        self.canvas_nsp.cd()
        self.hnsp_1.Draw()
        self.hnsp_0.Draw("same")
        self.hnsp_2.Draw("same")

        # Number of Space points per spill
        self.canvas_nsp_spill = ROOT.TCanvas("nsp_spill", "nsp_spill", 800, 800)
        self.canvas_nsp_spill.cd()
        for i in range (3):
            if i == 0:
                self.hnsp_spill[i].Draw()
            else:
                self.hnsp_spill[i].Draw("same")

        # Number of Space points per spill
        self.canvas_nsp_vs_spill = ROOT.TCanvas("nsp_vs_spill", 
                                              "nsp_vs_spill", 800, 900)
        self.canvas_nsp_vs_spill.Divide(1, 3)
        for i in range(3):
            self.canvas_nsp_vs_spill.cd(i+1)
            self.hnsp_vs_spill[i].Draw()

        # SP x 1d
        self.canvas_sp_x = ROOT.TCanvas("sp_x", "sp_x", 800, 800)
        self.canvas_sp_x.cd()
        self.hspslabx_1.Draw()
        self.hspslabx_0.Draw("same")
        self.hspslabx_2.Draw("same")

        # SP y 1d
        self.canvas_sp_y = ROOT.TCanvas("sp_y", "sp_y", 800, 800)
        self.hspslaby_1.Draw()
        self.hspslaby_0.Draw("same")
        self.hspslaby_2.Draw("same")

        # SP y vs x 2d
        self.canvas_sp_xy = []
        self.hspxy = []
        for i in range (3):
            cname = "sp_xy_%d" % (i)
            c_x = 800
            self.canvas_sp_xy.append(ROOT.TCanvas(cname, cname, c_x, c_x))
            self.canvas_sp_xy[i].cd()
            hname = "hspxy_%d" % (i)
            htitle = "tof%d: space points;SlabY;SlabX" % (i)
            nbins = 10
            edgelo = -0.5
            edgehi = 9.5
            if (i == 1):
                nbins = 7
                edgelo = -0.5
                edgehi = 6.5
            self.hspxy.append(ROOT.TH2F(hname, htitle, 
                                        nbins, edgelo, edgehi, 
                                        nbins, edgelo, edgehi))
            self.hspxy[i].Draw("text&&colz")

        # PMT Hits
        self.canvas_pmt = []
        for plane in range (2):
            for pmt in range (2):
                cname = "pmt%d%d" % (plane, pmt)
                c_x = 800
                self.canvas_pmt.append(ROOT.TCanvas(cname, cname, c_x, c_x))
                ind = 2*plane + pmt
                self.canvas_pmt[ind].cd()
                self.hpmthits[1][plane][pmt].Draw()
                for i in range (3):
                    self.hpmthits[i][plane][pmt].Draw("same")

        return True

    def update_histos(self):       
        """
        Updates histogram canvases. This is called only when the
        number of spills is divisible by the refresh rate.
        @param self Object reference.
        """

        #ll = ROOT.TLegend(0.6, 0.7, 0.89, 0.89)
        self.canvas_hits_x.cd()
        leg = self.canvas_hits_x.BuildLegend(0.6, 0.7, 0.89, 0.89)
        leg.Clear()
        leg.AddEntry(self.hslabhits[0][0], "TOF0", "l")
        leg.AddEntry(self.hslabhits[1][0], "TOF1", "l")
        leg.AddEntry(self.hslabhits[2][0], "TOF2", "l")
        all_max_y = []
        for hists in self.hslabhits[0:-1]:
            a_hist = hists[0]
            all_max_y.append(a_hist.GetBinContent(a_hist.GetMaximumBin()))
        for hists in self.hslabhits[0:-1]:
            hists[0].SetMaximum(max(all_max_y)*1.1+1)
        leg.SetBorderSize(0)
        leg.SetFillColor(0)
        leg.Draw()
        self.canvas_hits_x.Update()

        self.canvas_hits_y.cd()
        leg = self.canvas_hits_y.BuildLegend(0.6, 0.7, 0.89, 0.89)
        leg.Clear()
        all_max_y = []
        for hists in self.hslabhits[0:-1]:
            a_hist = hists[1]
            all_max_y.append(a_hist.GetBinContent(a_hist.GetMaximumBin()))
        for hists in self.hslabhits[0:-1]:
            hists[1].SetMaximum(max(all_max_y)*1.1+1)

        leg.AddEntry(self.hslabhits[0][1], "TOF0", "l")
        leg.AddEntry(self.hslabhits[1][1], "TOF1", "l")
        leg.AddEntry(self.hslabhits[2][1], "TOF2", "l")
        leg.SetBorderSize(0)
        leg.SetFillColor(0)
        leg.Draw()
        self.canvas_hits_y.Update()

        for plane in range (2):
            for pmt in range (2):
                ind = 2*plane + pmt
                leg = self.canvas_pmt[ind].BuildLegend(0.6, 0.7, 0.89, 0.89)
                leg.Clear()
                
                pnum = "Plane%d,PMT%d" % (plane, pmt)
                all_max_y = []
                for hists in self.hpmthits[0:-1]:
                    a_hist = hists[plane][pmt]
                    all_max_y.append(\
                                   a_hist.GetBinContent(a_hist.GetMaximumBin()))
                for hists in self.hpmthits[0:-1]:
                    hists[plane][pmt].SetMaximum(max(all_max_y)*1.1+1)
                leg.AddEntry(self.hpmthits[0][plane][pmt], "TOF0,"+pnum, "l")
                leg.AddEntry(self.hpmthits[1][plane][pmt], "TOF1,"+pnum, "l")
                leg.AddEntry(self.hpmthits[2][plane][pmt], "TOF2,"+pnum, "l")
                self.canvas_pmt[ind].Update()

        if self.have_sp is True:
            leg = self.canvas_nsp.BuildLegend(0.6, 0.7, 0.89, 0.89)
            leg.Clear()
            leg.AddEntry(self.hnsp_0, "TOF0", "l")
            leg.AddEntry(self.hnsp_1, "TOF1", "l")
            leg.AddEntry(self.hnsp_2, "TOF2", "l")
            self.canvas_nsp.Update()

            leg = self.canvas_nsp_spill.BuildLegend(0.6, 0.7, 0.89, 0.89)
            leg.Clear()
            leg.AddEntry(self.hnsp_spill[0], "TOF0", "l")
            leg.AddEntry(self.hnsp_spill[1], "TOF1", "l")
            leg.AddEntry(self.hnsp_spill[2], "TOF2", "l")
            all_max_y = []
            for i in range (3):
                a_hist = self.hnsp_spill[i]
                all_max_y.append(a_hist.GetBinContent(a_hist.GetMaximumBin()))
            for i in range (3):
                self.hnsp_spill[i].SetMaximum(max(all_max_y)*1.1+1)
            self.canvas_nsp_spill.Update()

            self.canvas_nsp_vs_spill.Update()


            all_max_y = []
            all_max_y.append(self.hspslabx_0.GetBinContent(+\
                             self.hspslabx_0.GetMaximumBin()))
            all_max_y.append(self.hspslabx_1.GetBinContent(+\
                             self.hspslabx_1.GetMaximumBin()))
            all_max_y.append(self.hspslabx_2.GetBinContent(+\
                             self.hspslabx_2.GetMaximumBin()))
            self.hspslabx_0.SetMaximum(max(all_max_y)*1.1+1)
            self.hspslabx_1.SetMaximum(max(all_max_y)*1.1+1)
            self.hspslabx_2.SetMaximum(max(all_max_y)*1.1+1)
            leg = self.canvas_sp_x.BuildLegend(0.6, 0.7, 0.89, 0.89)
            leg.Clear()
            leg.AddEntry(self.hspslabx_0, "TOF0", "l")
            leg.AddEntry(self.hspslabx_1, "TOF1", "l")
            leg.AddEntry(self.hspslabx_2, "TOF2", "l")
            self.canvas_sp_x.Update()
            all_max_y = []
            all_max_y.append(self.hspslaby_0.GetBinContent(+\
                             self.hspslaby_0.GetMaximumBin()))
            all_max_y.append(self.hspslaby_1.GetBinContent(+\
                             self.hspslaby_1.GetMaximumBin()))
            all_max_y.append(self.hspslaby_2.GetBinContent(+\
                             self.hspslaby_2.GetMaximumBin()))
            self.hspslaby_0.SetMaximum(max(all_max_y)*1.1+1)
            self.hspslaby_1.SetMaximum(max(all_max_y)*1.1+1)
            self.hspslaby_2.SetMaximum(max(all_max_y)*1.1+1)
            leg = self.canvas_sp_y.BuildLegend(0.6, 0.7, 0.89, 0.89)
            leg.Clear()
            leg.AddEntry(self.hspslaby_0, "TOF0", "l")
            leg.AddEntry(self.hspslaby_1, "TOF1", "l")
            leg.AddEntry(self.hspslaby_2, "TOF2", "l")
            self.canvas_sp_y.Update()
            for i in range (3):
                self.canvas_sp_xy[i].Update()
            for i in range (3):
                self.canvas_tof[i].Update()

    def get_histogram_images(self):       
        """
        Get histograms as JSON documents.
        @returns list of 3 JSON documents containing the images.
        """
        image_list = []

        # construct a list of histos to put in the root canvas
        histos = [self._ht01, self._ht02, self._ht12, self.hspslabx_0,
                  self.hspslabx_1, self.hspslabx_2, self.hspslaby_0, 
                  self.hspslaby_1, self.hspslaby_2, self.hnsp_0, self.hnsp_1,
                  self.hnsp_2]+self.hnsp_spill+\
                  self.hnsp_vs_spill
        for tof_station_hists in self.hpmthits: # histo for each pmt
            for tof_plane_hists in tof_station_hists:
                histos += tof_plane_hists
        for tof_station_hists in self.hslabhits: # histo for each plane
            histos += tof_station_hists
        histos += self.hspxy # hist for each station
        # now add them to the root/json document
        tag = __name__
        content = __name__
        doc = ReducePyROOTHistogram.get_root_doc(self, [], content, tag, histos)
        image_list.append(doc)


        # Slab Hits X
        # file label = tof_hit_x.eps
        tag = "tof_SlabHits_X"
        keywords = ["TOF", "raw", "hits"]
        description = "TOF Slab Hits X"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_hits_x)
        image_list.append(doc)

        # Slab Hits Y
        # file label = tof_hit_y.eps
        tag = "tof_SlabHits_Y"
        description = "TOF Slab Hits Y"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_hits_y)
        image_list.append(doc)

        # PMT hits
        # the files are labeled: tof_pmt00 tof_pmt01 tof_pmt10 tof_pmt11
        # the numbers stand for plane,pmt eg tof_pmt01 -> plane0, pmt1
        keywords = ["TOF", "PMT", "plane"]
        for plane in range (2):
            for pmt in range (2):
                ind = 2*plane + pmt
                tag = "tof_pmtHits_Plane%dPMT%d" % (plane, pmt)
                description = "TOF PMT Plane%d PMT%d" % (plane, pmt)
                doc = ReducePyROOTHistogram.get_image_doc( \
                    self, keywords, description, tag, self.canvas_pmt[ind])
                image_list.append(doc)

        # number of space points
        # file label = tof_nsp.eps
        tag = "tof_nSpacePoints"
        keywords = ["TOF", "space", "points"]
        description = "TOF Number of Space Points"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_nsp)
        image_list.append(doc)

        # number of space points per spill
        # file label = tof_nsp_spill.eps
        tag = "tof_nSpacePoints_spill"
        keywords = ["TOF", "spacepoints", "spill"]
        description = "TOF Number of Space Points in spill"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_nsp_spill)
        image_list.append(doc)

        # number of space points per spill
        # file label = tof_nsp_vs_spill.eps
        tag = "tof_nSpacePoints_vs_spill"
        keywords = ["TOF", "spacepoints", "spillnum"]
        description = "TOF Number of Space Points vs spill"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_nsp_vs_spill)
        image_list.append(doc)

        # Spacepoints X
        # file label = tof_sp_x.eps
        tag = "tof_SpacePoints_X"
        description = "TOF Space Points X"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_sp_x)
        image_list.append(doc)

        # Spacepoints Y
        # file label = tof_sp_y.eps
        tag = "tof_SpacePoints_Y"
        description = "TOF Space Points Y"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_sp_y)
        image_list.append(doc)

        # space point 2d output
        # the files are labeled: tof_xy_0, tof_xy_1, tof_xy_2 .eps
        for i in range (3):
            tag = "tof_SpacePoints_XY_tof%d" % (i)
            description = "TOF%d Space Points 2d" % (i)
            doc = ReducePyROOTHistogram.get_image_doc( \
                self, keywords, description, tag, self.canvas_sp_xy[i])
            image_list.append(doc)

        # time of flight between tof stations
        # files are labeled tof_time_01, tof_time_12, tof_time_02 .eps
        keywords = ["TOF", "time", "flight"]
        for i in range (3):
            if (i < 2):
                tag = "tof_time_tof%d-tof%d" % (i, i+1)
                description = "TOF%d->TOF%d Time" % (i, i+1)
            else:
                tag = "tof_time_tof0-tof2"
                description = "TOF0->TOF2 Time"
            doc = ReducePyROOTHistogram.get_image_doc( \
                self, keywords, description, tag, self.canvas_tof[i])
            image_list.append(doc)

        return image_list

    def _cleanup_at_death(self):
        """
        Reinitialise histograms at death and print out new (empty) images
        """
        self.__init_histos()
        self.get_histogram_images()
