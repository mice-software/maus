"""
ReducePyEMRPlot fills EMR histograms for plane charge, bar ToT, time,
draws them, refreshes the canvases and prints to eps at the end of run.
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
#pylint: disable = R0914
#pylint: disable = R0915
import ROOT
import array
import math
from ReducePyROOTHistogram import ReducePyROOTHistogram

class ReducePyEMRPlot(ReducePyROOTHistogram): # pylint: disable=R0902
    """
    ReducePyEMRPlot plots several EMR histograms.
    Currently the following histograms are filled:
    - Occupancy in the xz and yz projections
    - Beam profile at the entrance of the EMR in the xy plane
    - Beam depth profile from the SAPMT information

    - Range of the primary and secondary particles
    - Total ADC charge from the MAPMT and the SAPMT
    - Charge ratio of the first part of the track over the end part
    - Chi squared ln(1+chi_x^2+chi_y^2) vs plane density rho_P
    
    Histograms are drawn on 2 different canvases.
    The canvases are refreshed every N spills where N = refresh_rate
    set via refresh_rate data card value (see below).
   
    The default is to run ROOT in batch mode
    To run in interactive mode, set root_batch_mode = 0 in the data card
    (see below). 
    
    At the end of the run, the canvases are printed to eps files

    A sequence of 1 histogram is output as JSON documents of form:

    @verbatim
    {"image": {"keywords": [...list of image keywords...],
               "description":"...a description of the image...",
               "tag": TAG,
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}}
    @endverbatim

   
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
        # pid reconstruction enable
        self.enable_pid_recon = 0
        # has an end_of_run been processed?
        self.run_ended = False

        # Histogram initializations. they are defined explicitly in
        # init_occ_histos
        self.hoccupancy_x = None
        self.hoccupancy_y = None

        self.hbeam_profile = None

        self.hdepth_profile = None

        # init_pid_histos
        self.hrange_primary = None
        self.hrange_secondary = None

        self.htotal_charge_ma = None
        self.htotal_charge_sa = None

        self.hcharge_ratio_ma = None
        self.hcharge_ratio_sa = None

        self.hspread_vs_density = None

        # init legends
        self.leg_range = None
        self.leg_charge = None

        # init lines
        self.line_density = None
        self.line_chi2 = None

        # init text comments
        self.text_match_eff = None
        self.text_pid_stats = None

        # init canvas
        self.canvas_occ = None
        self.canvas_recon = None

        # root style
        self.cnv = None
        self.style = None

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
        if 'enable_pid_recon' in config_doc:
            self.enable_pid_recon = int(config_doc["enable_pid_recon"])
        # Initialize histograms, setup root canvases, and set root
        # styles.
        self.__init_histos()
        self.run_ended = False
        return True

    def _update_histograms(self, spill):
        """Update the Histograms """
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

        # Get EMR events & fill the relevant histograms.
        if data_spill and not self.get_events(spill): 
            raise ValueError("EMR events not in spill")

        # Get EMR reconstruction variables & fill the relevant histograms.
        if self.enable_pid_recon:
            if data_spill and not self.get_pid_recon(spill): 
                raise ValueError("EMR pid reconstruction not in spill")

        # Refresh canvases at requested frequency.
        #print self.refresh_rate
        if self.spill_count % self.refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return []

    def get_events(self, data):
        """ 
        Get the EMR events and update the occupancy histograms

        @param self Object reference.
        @param data Current spill data.
        @return True if no errors or False if no "emr_digits" in
        the spill.
        """
        if data.GetSpill().GetReconEventSize() == 0:
            raise ValueError("recon_events not in spill")
        reconevents = data.GetSpill().GetReconEvents()
        for evn in range(data.GetSpill().GetReconEventSize()):
            emr_event = reconevents[evn].GetEMREvent()
            if emr_event is None:
                raise ValueError("emr_event not in recon_events")

            totmax_x = 0
            totmax_y = 0
            x_pos = -999.0
            y_pos = -999.0

            emr_planes = emr_event.GetEMRPlaneHitArray()
            for plane in range(emr_planes.size()):
                plane_id = emr_planes[plane].GetPlane()
                charge = emr_planes[plane].GetCharge()
                # fill the depth profile of the EMR (plane charge)
                if charge:
                    self.hdepth_profile.Fill(plane_id)

                emr_bars = emr_planes[plane].GetEMRBarArray()
                for sbar in range(emr_bars.size()):
                    sbar_id = emr_bars[sbar].GetBar()

                    emr_hits = emr_bars[sbar].GetEMRBarHitArray()
                    for hit in range(emr_hits.size()):
                        tot = emr_hits[hit].GetTot()
                        # fill the occupancy of the EMR in the two projections
                        if tot:
                            bpx = array.array('d', [plane_id+sbar_id%2, \
                                                   plane_id+1-sbar_id%2, \
                                                   plane_id+sbar_id%2, \
                                                   plane_id+sbar_id%2])
                            bpy = array.array('d', [sbar_id-1, sbar_id, \
                                                   sbar_id+1, sbar_id-1])
                            if (plane_id % 2 == 0):
                                bin_id = self.hoccupancy_x\
                                    .FindBin(plane_id+0.5, sbar_id)
                                if (bin_id < 0):
                                    bin_id = self.hoccupancy_x\
                                        .AddBin(4, bpx, bpy)

                                i = self.hoccupancy_x.GetBinContent(bin_id)
                                self.hoccupancy_x.SetBinContent(bin_id, i+1)

                            else:
                                bin_id = self.hoccupancy_y\
                                    .FindBin(plane_id+0.5, sbar_id)
                                if (bin_id < 0):
                                    bin_id = self.hoccupancy_y\
                                       .AddBin(4, bpx, bpy)

                                i = self.hoccupancy_y.GetBinContent(bin_id)
                                self.hoccupancy_y.SetBinContent(bin_id, i+1)

                            # reconstruct the coordinates at the entrance
                            if (tot > totmax_x and plane_id == 0):
                                x_pos = (sbar_id-30)*17
                            if (tot > totmax_y and plane_id == 1):
                                y_pos = (sbar_id-30)*17

            # fill the beam profile at the entrance of the EMR
            if (x_pos > -999.0 and y_pos > -999.0):
                self.hbeam_profile.Fill(x_pos, y_pos)

        return True

    def get_pid_recon(self, data):
        """ 
        Get the EMR events and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "emr_digits" in
        the spill.
        """
        if data.GetSpill().GetReconEventSize() == 0:
            raise ValueError("recon_events not in spill")
        reconevents = data.GetSpill().GetReconEvents()
        for evn in range(data.GetSpill().GetReconEventSize()):
            emr_event = reconevents[evn].GetEMREvent()
            if emr_event is None:
                raise ValueError("emr_event not in recon_events")

            # Skip EMR events without a primary
            if not emr_event.GetHasPrimary():
                continue

            # Fill the range histograms
            self.hrange_primary.Fill(emr_event.GetRangePrimary())
            if emr_event.GetHasSecondary():
                self.hrange_secondary.Fill(emr_event.GetRangeSecondary())

            # Fill the charge histograms
            self.htotal_charge_ma.Fill(emr_event.GetTotalChargeMA())
            self.htotal_charge_sa.Fill(emr_event.GetTotalChargeSA())

            self.hcharge_ratio_ma.Fill(emr_event.GetChargeRatioMA())
            self.hcharge_ratio_sa.Fill(emr_event.GetChargeRatioSA())

            # Fill the PID TGraph, the muons are located in the bottom right
            density = emr_event.GetPlaneDensity()
            chi2 = emr_event.GetChi2()
            self.hspread_vs_density.Fill(density, math.log1p(chi2))

        # Efficiency of the track matching
        match_eff = self.hrange_secondary.GetEntries()\
                    / self.hrange_primary.GetEntries()
        string = 'Match eff. : %.3f' % (match_eff)
        self.text_match_eff\
            .SetText(.95*self.hrange_primary.GetXaxis().GetXmax(),
                     .65*self.hrange_primary.GetMaximum(),
                     string)

        # PID statistics
        n_entries = self.hspread_vs_density.GetEntries()
        n_mu = 0
        for i in range(self.hspread_vs_density.GetNbinsX(), 0, -1):
            if (self.hspread_vs_density.GetXaxis().GetBinCenter(i) < 0.9):
                break
            for j in range(1, self.hspread_vs_density.GetNbinsY()+1):
                if (self.hspread_vs_density.GetYaxis().GetBinCenter(j) > 1):
                    break
                n_mu = n_mu + self.hspread_vs_density.GetBinContent(i, j)
        string = 'Muon frac. : %.3f' % (n_mu/n_entries)
        self.text_pid_stats\
            .SetText(.95*self.hspread_vs_density.GetXaxis().GetXmax(),
                     .95*self.hspread_vs_density.GetYaxis().GetXmax(),
                     string)

        return True

    def __init_histos(self): #pylint: disable=R0201, R0914
        """
        Initialise ROOT to display histograms.

        @param self Object reference.
        """ 
        # white canvas
        self.cnv = ROOT.gROOT.SetStyle("Plain")
        
        #turn off stat box
        self.style = ROOT.gStyle.SetOptStat(0)
        
        #sensible color palette
        self.style = ROOT.gStyle.SetPalette(1)
        
        # xy grid on canvas
        self.style = ROOT.gStyle.SetPadGridX(1)
        self.style = ROOT.gStyle.SetPadGridY(1)

        # define occupancy histograms
        self.hoccupancy_x = ROOT.TH2Poly("emr_occupancy_x", \
                                         "Occupancy in the XZ plane", \
                                         0, 48, 0, 60)
        self.hoccupancy_x.GetXaxis().SetTitle("Plane ID")
        self.hoccupancy_x.GetYaxis().SetTitle("Bar ID")
        self.hoccupancy_y = ROOT.TH2Poly("emr_occupancy_y", \
                                         "Occupancy in the YZ plane", \
                                         0, 48, 0, 60)
        self.hoccupancy_y.GetXaxis().SetTitle("Plane ID")
        self.hoccupancy_y.GetYaxis().SetTitle("Bar ID")

        self.hbeam_profile = ROOT.TH2F("emr_beam_profile", \
                                       "Beam profile in the XY plane", \
                                       59, -500, 500, 59, -500, 500)
        self.hbeam_profile.GetXaxis().SetTitle("x [mm]")
        self.hbeam_profile.GetYaxis().SetTitle("y [mm]")

        self.hdepth_profile = ROOT.TH1F("emr_depth_profile", \
                                        "Beam profile along the Z axis", \
                                        48, 0, 48)
        self.hdepth_profile.GetXaxis().SetTitle("Plane ID")
        self.hdepth_profile.SetLineWidth(2)
 
        # define pid histograms
        self.hrange_primary = ROOT.TH1F("emr_primary_range", "EMR range", \
                                        48, 0, 840)
        self.hrange_primary.GetXaxis().SetTitle("Range [mm]")
        self.hrange_primary.SetLineWidth(2)
        self.hrange_secondary = ROOT.TH1F("emr_secondary_range", "EMR range", \
                                          48, 0, 840)
        self.hrange_secondary.SetLineWidth(2)
        self.hrange_secondary.SetLineColor(2)

        self.htotal_charge_ma = ROOT.TH1F("total_charge_ma", "Total charge", \
                                         100, 0, 10000)
        self.htotal_charge_ma.GetXaxis().SetTitle("Charge [ADC]")
        self.htotal_charge_ma.SetLineWidth(2)
        self.htotal_charge_sa = ROOT.TH1F("total_charge_sa", "Total charge", \
                                         100, 0, 10000)
        self.htotal_charge_sa.SetLineWidth(2)
        self.htotal_charge_sa.SetLineColor(2)

        self.hcharge_ratio_ma = ROOT.TH1F("charge_ratio_ma", "Charge ratio", \
                                         40, 0, 2)
        self.hcharge_ratio_ma.GetXaxis().SetTitle("Q_{4/5}/Q_{1/5}")
        self.hcharge_ratio_ma.SetLineWidth(2)
        self.hcharge_ratio_sa = ROOT.TH1F("charge_ratio_sa", "Charge ratio", \
                                         40, 0, 2)
        self.hcharge_ratio_sa.SetLineWidth(2)
        self.hcharge_ratio_sa.SetLineColor(2)

        self.hspread_vs_density = ROOT.TH2F("spread_vs_density", \
                                            "Normalised #chi^{2}\
					     against plane density", \
                                            20, 0, 1.0001, 20, 0, 5)
        self.hspread_vs_density.GetXaxis().SetTitle("#rho_{P}")
        self.hspread_vs_density.GetYaxis()\
            .SetTitle("ln(1 + #chi_{X}^{2}/N + #chi_{Y}^{2}/N)")

        # define legends
        self.leg_range = ROOT.TLegend(0.65, 0.7, 0.88, 0.88)
        self.leg_range.SetLineColor(0)
        self.leg_range.SetFillColor(0)
        self.leg_range.AddEntry(self.hrange_primary, "Primary", "l")
        self.leg_range.AddEntry(self.hrange_secondary, "Secondary", "l")

        self.leg_charge = ROOT.TLegend(0.65, 0.7, 0.88, 0.88)
        self.leg_charge.SetLineColor(0)
        self.leg_charge.SetFillColor(0)
        self.leg_charge.AddEntry(self.htotal_charge_ma, "MAPMT", "l")
        self.leg_charge.AddEntry(self.htotal_charge_sa, "SAPMT", "l")

        # define lines
        self.line_density = ROOT.TLine(.9, 0, .9, 1)
        self.line_density.SetLineWidth(2)
        self.line_chi2 = ROOT.TLine(.9, 1, 1, 1)
        self.line_chi2.SetLineWidth(2)

        # define text comments
        self.text_match_eff = ROOT.TText()
        self.text_match_eff.SetTextAlign(32) 
        self.text_match_eff.SetTextSize(0.05)

        self.text_pid_stats = ROOT.TText()
        self.text_pid_stats.SetTextAlign(32) 
        self.text_pid_stats.SetTextSize(0.05)

        # Create canvases
        #
        # Draw() of histos has to be done only once
        # for updating the histograms, just Modified() and Update() on canvases
        # the update/refresh is done in update_histos()

        self.canvas_occ = \
            ROOT.TCanvas("emr_occupancy", "emr_occupancy", 1000, 1000)
        self.canvas_occ.Divide(2, 2)
        self.canvas_occ.cd(1)
        self.hoccupancy_x.Draw("COLZ L")
        self.canvas_occ.cd(2)
        self.hoccupancy_y.Draw("COLZ L")
        self.canvas_occ.cd(3)
        ROOT.gStyle.SetOptStat("RM")
        ROOT.gStyle.SetStatX(.9)
        ROOT.gStyle.SetStatY(.9)
        ROOT.gStyle.SetStatW(.3)
        ROOT.gStyle.SetStatH(.1) 
        self.hbeam_profile.SetStats()
        self.hbeam_profile.Draw("COLZ")
        self.canvas_occ.cd(4)
        self.hdepth_profile.Draw("")

        if self.enable_pid_recon:
            self.canvas_recon = \
                ROOT.TCanvas("emr_recon", "emr_recon", 1200, 900)
            self.canvas_recon.Divide(2, 2)
            self.canvas_recon.cd(1) # Range canvas
            self.hrange_primary.Draw("")
            self.hrange_secondary.Draw("SAME")
            self.leg_range.Draw("SAME")
            self.text_match_eff.Draw("SAME")
            self.canvas_recon.cd(2) # Total charge canvas
            self.htotal_charge_ma.Draw()
            self.htotal_charge_sa.Draw("SAME")
            self.leg_charge.Draw("SAME")
            self.canvas_recon.cd(3) # Charge ratio canvas
            self.hcharge_ratio_ma.Draw()
            self.hcharge_ratio_sa.Draw("SAME")
            self.leg_charge.Draw("SAME")
            self.canvas_recon.cd(4) # PID variables canvas
            ROOT.gPad.SetLogz()
            self.hspread_vs_density.Draw("COLZ")
            self.text_pid_stats.Draw("SAME")
            self.line_density.Draw("SAME")
            self.line_chi2.Draw("SAME")


        return True

    def update_histos(self):       
        """
        Updates histogram canvases. This is called only when the
        number of spills is divisible by the refresh rate.
        @param self Object reference.
        """
        for i in range(1, 5):
            self.canvas_occ.cd(i).Modified()
        self.canvas_occ.Update()

        if self.enable_pid_recon:
            for i in range(1, 5):
                self.canvas_recon.cd(i).Modified()
            self.canvas_recon.Update()


    def get_histogram_images(self):       
        """
        Get histograms as JSON documents.
        @returns list of 1 JSON document containing the images.
        """
        image_list = []
         
        tag = "EMR_occupancy"
        keywords = ["EMR", "Occupancy"]
        description = "EMR Occupancy"
        doc_occ = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_occ)
        image_list.append(doc_occ)

        if self.enable_pid_recon:
            tag = "EMR_recon"
            keywords = ["EMR", "Reconstructed"]
            description = "EMR Reconstruction Output"
            doc_recon = ReducePyROOTHistogram.get_image_doc( \
                self, keywords, description, tag, self.canvas_recon)
            image_list.append(doc_recon)
        
        return image_list

    def _cleanup_at_death(self):
        """
        Reinitialise histograms at death and print out new (empty) images
        """
        self.__init_histos()
        self.get_histogram_images()
