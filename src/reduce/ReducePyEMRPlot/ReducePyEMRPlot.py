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
#pylint: disable = R0915
import ROOT
from ReducePyROOTHistogram import ReducePyROOTHistogram

class ReducePyEMRPlot(ReducePyROOTHistogram): # pylint: disable=R0902
    """
    ReducePyEMRPlot plots several EMR histograms.
    Currently the following histograms are filled:
    - EMR primary range
    
    Histograms are drawn on different canvases.
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
        # Histogram initializations. they are defined explicitly in
        # init_histos.
        self.hrange_primary = None
        self.hrange_secondary = None

        self.htotalcharge_ma = None
        self.htotalcharge_sa = None

        self.hchargeratio_ma = None
        self.hchargeratio_sa = None

        self.hspread_vs_density = None

        self.hoccupancy_x = None
        self.hoccupancy_y = None

        #init legends.
        self.leg_range = None
        self.leg_charge = None

        #init text comments
        self.text_match_eff = None

        # init canvas.
        self.canvas_recon = None
        self.canvas_occ = None

        # Has an end_of_run been processed?
        self.run_ended = False

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
        # Initialize histograms, setup root canvases, and set root
        # styles.
        self.__init_histos()
        self.run_ended = False
        return True

    def _update_histograms(self, spill):
        """Update the Histograms """
        if not spill.has_key("daq_event_type"):
            raise ValueError("No event type")
        if spill["daq_event_type"] == "end_of_run":
            if (not self.run_ended):
                self.update_histos()
                self.run_ended = True
                return self.get_histogram_images()
            else:
                return []

        # do not try to get data from start/end spill markers
        data_spill = True
        if spill["daq_event_type"] == "start_of_run" \
              or spill["daq_event_type"] == "start_of_burst" \
              or spill["daq_event_type"] == "end_of_burst":
            data_spill = False

        # Get EMR events & fill the relevant histograms.
        if data_spill and not self.get_events(spill): 
            raise ValueError("EMR events not in spill")

        # Refresh canvases at requested frequency.
        #print self.refresh_rate
        if self.spill_count % self.refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return []
        

    def get_events(self, spill):
        """ 
        Get the EMR events and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "emr_digits" in
        the spill.
        """
        if 'recon_events' not in spill:
            raise ValueError("recon_events not in spill")
        for evn in range(len(spill['recon_events'])):
            if 'emr_event' not in spill['recon_events'][evn]:
                raise ValueError("emr_event not in recon_events")

            # Skip EMR events without a primary
            if not spill['recon_events'][evn]['emr_event']['has_primary']:
                continue

            # Fill the range histograms
            self.hrange_primary.Fill(spill['recon_events'][evn]\
                                          ['emr_event']['range_primary'])
            if spill['recon_events'][evn]['emr_event']['has_secondary']:
                self.hrange_secondary.Fill(spill['recon_events'][evn]\
                                                ['emr_event']
                                                ['range_secondary'])

            # Fill the charge histograms
            self.htotalcharge_ma.Fill(spill['recon_events'][evn]\
                                           ['emr_event']['total_charge_MA'])
            self.htotalcharge_sa.Fill(spill['recon_events'][evn]\
                                           ['emr_event']['total_charge_SA'])

            self.hchargeratio_ma.Fill(spill['recon_events'][evn]\
                                           ['emr_event']['charge_ratio_MA'])
            self.hchargeratio_sa.Fill(spill['recon_events'][evn]\
                                           ['emr_event']['charge_ratio_SA'])

            # Fill the PID histograms, the last bin in Y is the overflow bin
            chi2 = spill['recon_events'][evn]['emr_event']['chi2']
            self.hspread_vs_density.Fill(spill['recon_events'][evn]\
                                              ['emr_event']['plane_density'],\
                                         chi2)

            # Efficiency of the track matching
            match_eff = self.hrange_secondary.GetEntries()\
                        / self.hrange_primary.GetEntries()
            string = 'Match Eff. : %.3f' % (match_eff)
            self.text_match_eff\
                .SetText(self.hrange_primary.GetXaxis().GetXmax(),
                         0.65*self.hrange_primary.GetMaximum(),
                         string)

            if (chi2 > 5.1):
                self.hspread_vs_density.Fill(spill['recon_events'][evn]\
                                                  ['emr_event']\
                                                  ['plane_density'], 5.05)


            for plane in range(len(spill['recon_events'][evn]['emr_event']\
                                        ['emr_plane_hits'])):
                plane_id = spill['recon_events'][evn]['emr_event']\
                                ['emr_plane_hits'][plane]['plane']
                for sbar in range(len(spill['recon_events'][evn]['emr_event']\
                                     ['emr_plane_hits'][plane]['emr_bars'])):
                    sbar_id = spill['recon_events'][evn]['emr_event']\
                                  ['emr_plane_hits'][plane]['emr_bars']\
                                  [sbar]['bar']
                    for hit in range(len(spill['recon_events'][evn]\
                                         ['emr_event']['emr_plane_hits']\
                                         [plane]['emr_bars'][sbar]\
                                         ['emr_bar_hits'])):
                        tot = spill['recon_events'][evn]['emr_event']\
                                   ['emr_plane_hits'][plane]['emr_bars']\
                                   [sbar]['emr_bar_hits'][hit]['tot']
                        if tot:
                            if (plane_id % 2 == 0):
                                self.hoccupancy_x.Fill(plane_id, sbar_id)
                            else:
                                self.hoccupancy_y.Fill(plane_id, sbar_id)

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
 
        # define histograms
        self.hrange_primary = ROOT.TH1F("emr_primary_range", "EMR range", \
                                        48, 0, 840)
        self.hrange_primary.GetXaxis().SetTitle("Range [mm]")
        self.hrange_primary.SetLineWidth(2)
        self.hrange_secondary = ROOT.TH1F("emr_secondary_range", "EMR range", \
                                          48, 0, 840)
        self.hrange_secondary.SetLineWidth(2)
        self.hrange_secondary.SetLineColor(2)

        self.htotalcharge_ma = ROOT.TH1F("total_charge_ma", "Total charge", \
                                        100, 0, 10000)
        self.htotalcharge_ma.GetXaxis().SetTitle("Q_{MA} [ADC]")
        self.htotalcharge_ma.SetLineWidth(2)
        self.htotalcharge_sa = ROOT.TH1F("total_charge_sa", "Total charge", \
                                        100, 0, 10000)
        self.htotalcharge_sa.SetLineWidth(2)
        self.htotalcharge_sa.SetLineColor(2)

        self.hchargeratio_ma = ROOT.TH1F("charge_ratio_ma", "Charge ratio", \
                                        40, 0, 2)
        self.hchargeratio_ma.GetXaxis().SetTitle("Q_{4/5}/Q_{1/5}")
        self.hchargeratio_ma.SetLineWidth(2)
        self.hchargeratio_sa = ROOT.TH1F("charge_ratio_sa", "Charge ratio", \
                                        40, 0, 2)
        self.hchargeratio_sa.SetLineWidth(2)
        self.hchargeratio_sa.SetLineColor(2)

        self.hspread_vs_density = ROOT.TH2F("emr_spread_vs_density", \
                                  "Normalised #chi^{2} against plane density", \
                                  20, 0, 1.001, 51, 0, 5.1)
        self.hspread_vs_density.GetXaxis().SetTitle("#rho_{P}")
        self.hspread_vs_density.GetYaxis()\
            .SetTitle("#chi_{X}^{2}/N + #chi_{Y}^{2}/N")

        self.hoccupancy_x = ROOT.TH2F("emr_occupancy_x", "X projection", \
                                      48, -0.5, 47.5, 60, -0.5, 59.5)
        self.hoccupancy_y = ROOT.TH2F("emr_occupancy_y", "Y projection", \
                                      48, -0.5, 47.5, 60, -0.5, 59.5)

        # define legends
        self.leg_range = ROOT.TLegend(0.65, 0.7, 0.88, 0.88)
        self.leg_range.SetLineColor(0)
        self.leg_range.SetFillColor(0)
        self.leg_range.AddEntry(self.hrange_primary, "Primary", "l")
        self.leg_range.AddEntry(self.hrange_secondary, "Secondary", "l")

        self.leg_charge = ROOT.TLegend(0.65, 0.7, 0.88, 0.88)
        self.leg_charge.SetLineColor(0)
        self.leg_charge.SetFillColor(0)
        self.leg_charge.AddEntry(self.htotalcharge_ma, "MAPMT", "l")
        self.leg_charge.AddEntry(self.htotalcharge_sa, "SAPMT", "l")

        # define text comment
        self.text_match_eff = ROOT.TText()
        self.text_match_eff.SetTextAlign(32) 
        self.text_match_eff.SetTextSize(0.05)

        # Create canvases
        #
        # Draw() of histos has to be done only once
        # for updating the histograms, just Modified() and Update() on canvases
        # the update/refresh is done in update_histos()
        
        self.canvas_recon = \
            ROOT.TCanvas("emr_recon", "emr_recon", 1200, 900)
        self.canvas_recon.Divide(2, 2)
        self.canvas_recon.cd(1) # Range canvas
        self.hrange_primary.Draw()
        self.hrange_secondary.Draw("SAME")
        self.leg_range.Draw("SAME")
        self.text_match_eff.Draw("SAME")
        self.canvas_recon.cd(2) # Total charge canvas
        self.htotalcharge_ma.Draw()
        self.htotalcharge_sa.Draw("SAME")
        self.leg_charge.Draw("SAME")
        self.canvas_recon.cd(3) # Charge ratio canvas
        self.hchargeratio_ma.Draw()
        self.hchargeratio_sa.Draw("SAME")
        self.leg_charge.Draw("SAME")
        self.canvas_recon.cd(4) # PID variables canvas
        self.hspread_vs_density.Draw("COLZ")


        self.canvas_occ = \
            ROOT.TCanvas("emr_occupancy", "emr_occupancy", 1000, 600)
        self.canvas_occ.Divide(2, 1)
        self.canvas_occ.cd(1)
        self.hoccupancy_x.Draw("COLZ")
        self.canvas_occ.cd(2)
        self.hoccupancy_y.Draw("COLZ")

        return True

    def update_histos(self):       
        """
        Updates histogram canvases. This is called only when the
        number of spills is divisible by the refresh rate.
        @param self Object reference.
        """
        self.canvas_recon.Update()
        self.canvas_occ.Update()

    def get_histogram_images(self):       
        """
        Get histograms as JSON documents.
        @returns list of 1 JSON document containing the images.
        """
        image_list = []

        tag = "EMR_recon"
        keywords = ["EMR", "Reconstructed"]
        description = "EMR Reconstruction Output"
        doc_recon = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_recon)
        image_list.append(doc_recon)
         
        tag = "EMR_occupancy"
        keywords = ["EMR", "Occupancy"]
        description = "EMR Occupancy"
        doc_occ = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_occ)
        image_list.append(doc_occ)
        
        return image_list

    def _cleanup_at_death(self):
        """
        Reinitialise histograms at death and print out new (empty) images
        """
        self.__init_histos()
        self.get_histogram_images()
