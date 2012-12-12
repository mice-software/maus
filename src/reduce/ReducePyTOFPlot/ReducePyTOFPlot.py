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
        self.hslabhits = None

        self.canvas_tof = None
        self.canvas_pmt = None
        self.canvas_hits_x = None
        self.canvas_hits_y = None
        self.canvas_nsp = None
        self.canvas_sp_x = None
        self.canvas_sp_y = None
        self.canvas_sp_xy = None
        # Has an end_of_run been processed?
        self.run_ended = False

    def _configure_at_birth(self, config_doc):
        """
        Configure worker from data cards. Overrides super-class method. 
        @param self Object reference.
        @param config_doc JSON document.
        @returns True if configuration succeeded.
        """
        # Read in configuration flags and parameters - these will
        # overwrite whatever defaults were set in __init__.
        if 'refresh_rate' in config_doc:
            self.refresh_rate = int(config_doc["refresh_rate"])
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
        if not spill.has_key("daq_event_type"):
            raise ValueError("No event type")
        if spill["daq_event_type"] == "end_of_run":
            if (not self.run_ended):
                self.update_histos()
                self.run_ended = True
                return self.get_histogram_images()
            else:
                return [{}]
        # elif spill["daq_event_type"] != "physics_event":
        #    return spill

        # do not try to get data from start/end spill markers
        data_spill = True
        if spill["daq_event_type"] == "start_of_run" \
              or spill["daq_event_type"] == "start_of_burst" \
              or spill["daq_event_type"] == "end_of_burst":
            data_spill = False

        # Get TOF slab hits & fill the relevant histograms.
        if data_spill and not self.get_slab_hits(spill): 
            raise ValueError("slab_hits not in spill")

        # Get TOF space points & fill histograms.
        if data_spill and not self.get_space_points(spill):
            raise ValueError("space_points not in spill")

        # Refresh canvases at requested frequency.
        if self.spill_count % self.refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return [spill]

    def get_slab_hits(self, spill):
        """ 
        Get the TOF slab hits and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "slab_hits" in
        the spill.
        """
        if 'recon_events' not in spill:
            raise ValueError("recon_events not in spill")
        # print 'nevt = ', len(spill['recon_events']), spill['recon_events']
        for evn in range(len(spill['recon_events'])):
            if 'tof_event' not in spill['recon_events'][evn]:
                # print 'no tof event'
                raise ValueError("tof_event not in recon_events")
            # Return if we cannot find slab_hits in the event.
            if 'tof_slab_hits' not in spill['recon_events'][evn]['tof_event']:
                return False

            slabhits = spill['recon_events'][evn]['tof_event']['tof_slab_hits']

            # setup the detectors for which we want to look at hits
            dets = ['tof0', 'tof1', 'tof2']
                   
            # loop over detector stations ie tof0,tof1,tof2
            for index, station in enumerate(dets):
                # leave if we cannot find slab hits for this detector
                if station not in slabhits:
                    continue
                dethits = slabhits[station]
                # print 'idx,stn: ',index,station,len(dethits),dethits
                # loop over all slab hits for this detector station
                if dethits == None:
                    continue
                for i in range(len(dethits)):
                    # make sure it is not null
                    if (dethits[i]):
                        # wrong. no further loop. ie no j
                        # for j in range(len(dethits[i])): #loop over planes
                        pos = dethits[i]['slab']
                        plane_num = dethits[i]["plane"]
                        # make sure the plane number is valid so 
                        # we don't overflow bounds
                        if plane_num < 0 or plane_num > 1:
                            return False
                        self.hslabhits[index][plane_num].Fill(pos)
                        # plane 0, pmt0 hit for this slab
                        if ("pmt0" in dethits[i]):
                            self.hpmthits[index][plane_num][0].Fill(pos)
                        # plane 0, pmt1 hit for this slab
                        if ("pmt1" in dethits[i]):
                            self.hpmthits[index][plane_num][1].Fill(pos)
        return True

    def get_space_points(self, spill):
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
        if 'recon_events' not in spill:
            # print 'no reco'
            return False

        for evn in range(len(spill['recon_events'])):
            if 'tof_event' not in spill['recon_events'][evn]:
                # print 'no tof event'
                return False
                # print spill['recon_events'][evn]['tof_event']
                # Return if we cannot find slab_hits in the spill.
            if 'tof_space_points' not in \
                  spill['recon_events'][evn]['tof_event']:
                return False
            space_points = \
                  spill['recon_events'][evn]['tof_event']['tof_space_points']

            # print 'evt: ', evn, ' nsp: ',len(space_points)
            # if there are no TOF0,1,2 space point objects, return false
            sp_tof0 = None
            sp_tof1 = None
            sp_tof2 = None

            if 'tof0' in space_points:
                sp_tof0 = space_points['tof0']

            if 'tof1' in space_points:
                sp_tof1 = space_points['tof1']

            if 'tof2' in space_points:
                sp_tof2 = space_points['tof2']

            # print 'nsp012= ', len(sp_tof0), len(sp_tof1), len(sp_tof2)
         
            # TOF0
            if sp_tof0:
                # print '..evt ',evn,' nsp0: ',len(sp_tof0)
                self.hnsp_0.Fill(len(sp_tof0))
                for i in range(len(sp_tof0)):
                    if sp_tof0[i]:
                        # print 'nsp0: ',i,sp_tof0[i]
                        spnt_x = sp_tof0[i]["slabX"]
                        spnt_y = sp_tof0[i]["slabY"]
                        self.hspxy[0].Fill(spnt_x, spnt_y)
                        self.hspslabx_0.Fill(spnt_x)
                        self.hspslaby_0.Fill(spnt_y)
                        # print '... ', i
                        if sp_tof1 is not None :
                            if len(sp_tof0)==1 and len(sp_tof1)==1:
                                t_0 = sp_tof0[i]["time"]
                                t_1 = sp_tof1[i]["time"]
            else:
                self.hnsp_0.Fill(0)           

            # TOF 2
            if sp_tof2:
                # print '..evt ', evn, ' nsp2: ', len(sp_tof2)
                self.hnsp_2.Fill(len(sp_tof2))
                for i in range(len(sp_tof2)):
                    if sp_tof2[i]:
                        spnt_x = sp_tof2[i]["slabX"]
                        spnt_y = sp_tof2[i]["slabY"]
                        self.hspxy[2].Fill(spnt_x, spnt_y)
                        self.hspslabx_2.Fill(spnt_x)
                        self.hspslaby_2.Fill(spnt_y)
                        if sp_tof1 is not None :
                            # print '2&1: ', len(sp_tof1)
                            if len(sp_tof2)==1 and len(sp_tof1)==1:
                                t_2 = sp_tof2[i]["time"]
                                t_1 = sp_tof1[i]["time"]
                                self._ht12.Fill(t_2-t_1)
                                # print 'tof: ', t_2-t_1

                        if sp_tof0 is not None :
                            if len(sp_tof2)==1 and len(sp_tof0)==1:
                                t_2 = sp_tof2[i]["time"]
                                t_0 = sp_tof0[i]["time"]
                                self._ht02.Fill(t_2-t_0)
            else:
                self.hnsp_2.Fill(0)
            # TOF 1
            if sp_tof1:
                # print '..evt ',evn,' nsp1: ',len(sp_tof1)
                self.hnsp_1.Fill(len(sp_tof1))
                for i in range(len(sp_tof1)):
                    if sp_tof1[i]:
                        # print 'nsp1: ', i, sp_tof1[i]
                        spnt_x = sp_tof1[i]["slabX"]
                        spnt_y = sp_tof1[i]["slabY"]
                        self.hspxy[1].Fill(spnt_x, spnt_y)
                        self.hspslabx_1.Fill(spnt_x)
                        self.hspslaby_1.Fill(spnt_y)
                        # print '... ', i
                        if sp_tof0 is not None :
                            if len(sp_tof1)==1 and len(sp_tof0)==1:
                                # print '>>>> ok'
                                t_0 = sp_tof0[i]["time"]
                                t_1 = sp_tof1[i]["time"]
                                self._ht01.Fill(t_1-t_0)
                                # print 'tof01: ', t_1-t_0
            else:
                self.hnsp_1.Fill(0)           
        return True

    def __init_histos(self): #pylint: disable=R0201, R0914
        """
        Initialise ROOT to display histograms.

        @param self Object reference.
        """ 
        # have root run quietly without verbose informationals
        ROOT.gErrorIgnoreLevel = 1001

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
        self._ht12 = ROOT.TH1F("ht12", "TOF1->2;Time (ns);;", 200, 0, 20)
        self._ht02 = ROOT.TH1F("ht02", "TOF0->2;Time (ns);;", 200, 30, 50)
        
        
        self.hspslabx_0 = ROOT.TH1F("spx0", "SpacePoints X-plane;SlabX;",
                                     10, -0.5, 9.5)
        self.hspslaby_0 = ROOT.TH1F("spy0", "SpacePoints Y-plane;SlabY;",
                                     10, -0.5, 9.5)
        self.hspslabx_1 = ROOT.TH1F("spx1", "SpacePoints X-plane;SlabX;",
                                     10, -0.5, 9.5)
        self.hspslaby_1 = ROOT.TH1F("spy1", "SpacePoints Y-plane;SlabY;",
                                     10, -0.5, 9.5)
        self.hspslabx_1.SetFillColor(2)
        self.hspslabx_1.SetLineColor(2)
        self.hspslaby_1.SetFillColor(2) 
        self.hspslaby_1.SetLineColor(2) 
        self.hspslabx_2 = ROOT.TH1F("spx2", "SpacePoints X-plane;SlabX;",
                                     10, -0.5, 9.5)
        self.hspslaby_2 = ROOT.TH1F("spy2", "SpacePoints Y-plane;SlabY;",
                                     10, -0.5, 9.5)
        self.hspslabx_2.SetFillColor(4)
        self.hspslabx_2.SetLineColor(4)
        self.hspslaby_2.SetFillColor(4) 
        self.hspslaby_2.SetLineColor(4) 
        
        self.hnsp_0 = ROOT.TH1F("hnsp_0", 
                                "#Space Points;#space points in event;;",
                                 4, -0.5, 3.5)
        self.hnsp_0.SetLineColor(1)
        self.hnsp_1 = ROOT.TH1F("hnsp_1", 
                                "#Space Points;#space points in event;;",
                                 4, -0.5, 3.5)
        self.hnsp_1.SetFillColor(2)
        self.hnsp_1.SetLineColor(2)
        #self.hnsp_1.SetFillStyle(4000)
        self.hnsp_2 = ROOT.TH1F("hnsp_2", 
                                 "#Space Points;#space points in event;;",
                                 4, -0.5, 3.5)
        self.hnsp_2.SetFillColor(4)
        self.hnsp_2.SetLineColor(4)

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
                    self.hslabhits[i][j].SetFillColor(2*i)
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
                        self.hpmthits[i][j][k].SetFillColor(2*i)
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
        self.canvas_hits_x = ROOT.TCanvas("hits_x", "hits_x", 800, 800)
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
            htitle = "tof%d: space points;SlabX;SlabY" % (i)
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
        leg.SetBorderSize(0)
        leg.SetFillColor(0)
        leg.Draw()
        self.canvas_hits_x.Update()

        self.canvas_hits_y.cd()
        leg = self.canvas_hits_y.BuildLegend(0.6, 0.7, 0.89, 0.89)
        leg.Clear()
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
                leg.AddEntry(self.hpmthits[0][plane][pmt], "TOF0,"+pnum, "l")
                leg.AddEntry(self.hpmthits[1][plane][pmt], "TOF1,"+pnum, "l")
                leg.AddEntry(self.hpmthits[2][plane][pmt], "TOF2,"+pnum, "l")
                self.canvas_pmt[ind].Update()

        leg = self.canvas_nsp.BuildLegend(0.6, 0.7, 0.89, 0.89)
        leg.Clear()
        leg.AddEntry(self.hnsp_0, "TOF0", "l")
        leg.AddEntry(self.hnsp_1, "TOF1", "l")
        leg.AddEntry(self.hnsp_2, "TOF2", "l")
        self.canvas_nsp.Update()

        leg = self.canvas_sp_x.BuildLegend(0.6, 0.7, 0.89, 0.89)
        leg.Clear()
        leg.AddEntry(self.hspslabx_0, "TOF0", "l")
        leg.AddEntry(self.hspslabx_1, "TOF1", "l")
        leg.AddEntry(self.hspslabx_2, "TOF2", "l")
        self.canvas_sp_x.Update()
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
        Reinitialise histograms at death
        """
        self.__init_histos()
