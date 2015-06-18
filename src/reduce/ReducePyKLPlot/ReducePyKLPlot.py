# -*- coding: utf-8 -*-
"""
ReducePyKLPlot fills KL histograms for digits and slab hits,
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

class ReducePyKLPlot(ReducePyROOTHistogram): # pylint: disable=R0902
    """
    ReducePyKLPlots plots several KL histograms.
    Currently the following histograms are filled:
    - KL charge in ADC units for every digit.
    - KL charge product for every cell.
    - Beam profile in Y direction.
    - 2-dim map of kl digits.
    
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

        self.hadc = None
        self.hadc_product = None
        self.hprofile = None
        self.digitkl = None

        self.canvas_kl = None
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
        # elif spill["daq_event_type"] != "physics_event":
        #    return spill

        # Get KL digits & fill the relevant histograms.
        if not self.get_digits(spill): 
            raise ValueError("kl digits not in spill")

        # Get KL cell hits & fill the relevant histograms.
        if not self.get_cell_hits(spill): 
            raise ValueError("cell hits not in spill")

        # Refresh canvases at requested frequency.
        #print self.refresh_rate
        if self.spill_count % self.refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return []

    def get_cell_hits(self, spill):
        """ 
        Get the KL cell hits and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "cell_hits" in
        the spill.
        """
        if 'recon_events' not in spill:
            raise ValueError("recon_events not in spill")
        #print 'nevt = ', len(spill['recon_events']), spill['recon_events']
        for evn in range(len(spill['recon_events'])):
            if 'kl_event' not in spill['recon_events'][evn]:
                #print 'no kl event'
                raise ValueError("kl_event not in recon_events")
            # Return if we cannot find cell_hits in the event.
            if 'kl_cell_hits' not in spill['recon_events'][evn]['kl_event']:
                return False

            cellhits = spill['recon_events'][evn]['kl_event']['kl_cell_hits']
            if 'kl' not in cellhits:
                continue
            kl_cellhit = cellhits['kl']
            if kl_cellhit == None:
                continue
            #print kl_cellhit 
            for i in range(len(kl_cellhit)):
                # make sure it is not null
                if (kl_cellhit[i]):
                    prod = kl_cellhit[i]['charge_product']
                    self.hadc_product.Fill(prod)
        return True

    def get_digits(self, spill):
        """ 
        Get the KL digits and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "kl_digits" in
        the spill.
        """
        if 'recon_events' not in spill:
            #print 'no reco'
            return False

        for evn in range(len(spill['recon_events'])):
            if 'kl_event' not in spill['recon_events'][evn]:
                #print 'no kl event'
                return False
                # Return if we cannot find kl_digits in the spill.
            if 'kl_digits' not in \
                  spill['recon_events'][evn]['kl_event']:
                return False
            kl_digit = \
                  spill['recon_events'][evn]['kl_event']['kl_digits']
            if 'kl' not in kl_digit:
                continue   

            digit = kl_digit['kl']
            if digit == None:
                continue
            #print digit
             
            for i in range(len(digit)):
                if (digit[i]):
                    charge = digit[i]["charge_mm"]
                    cell = digit[i]["cell"]
                    side = digit[i]["pmt"]
                    self.hadc.Fill(charge)
                    self.hprofile.Fill(cell)
                    self.digitkl.Fill(cell, side)
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
        self.hadc = ROOT.TH1F("h1", "ADC", 100, 0, 5000)
        self.hadc.GetXaxis().SetTitle("ADC")
        self.hadc_product = ROOT.TH1F("h2", "ADC Product", 100, 0, 5000)
        self.hadc_product.GetXaxis().SetTitle("ADC product")
        self.hprofile = ROOT.TH1F("h3", "Beam Y-profile", 21, -0.5, 20.5)
        self.hprofile.GetXaxis().SetTitle("Cell")
        self.digitkl = ROOT.TH2F("h4", "Digits", 21, -0.5, 20.5, 2, -0.5, 1.5)
        self.digitkl.GetXaxis().SetTitle("Cell")
        self.digitkl.GetYaxis().SetTitle("PMT")
        self.digitkl.GetYaxis().SetBinLabel(1, "0")
        self.digitkl.GetYaxis().SetBinLabel(2, "1")
        # Create canvases
        #
        # Draw() of histos has to be done only once
        # for updating the histograms, just Modified() and Update() on canvases
        # the update/refresh is done in update_histos()
        
        self.canvas_kl = ROOT.TCanvas("kl", "kl")
        self.canvas_kl.Divide(2, 2)
        self.canvas_kl.cd(1)
        self.hadc.Draw()
        self.canvas_kl.cd(2)
        self.hadc_product.Draw()
        self.canvas_kl.cd(3)
        self.hprofile.Draw()
        self.canvas_kl.cd(4)
        self.digitkl.Draw("text&&colz")

        
        return True

    def update_histos(self):       
        """
        Updates histogram canvases. This is called only when the
        number of spills is divisible by the refresh rate.
        @param self Object reference.
        """
        #self.canvas_kl.cd(1)
        #self.hadc.Draw()
        #self.canvas_kl.cd(2)
        #self.hadc_product.Draw()
        #self.canvas_kl.cd(3)
        #self.hprofile.Draw()
        #self.canvas_kl.cd(4)
        #self.digitkl.Draw("text&&colz")
        self.canvas_kl.Update()

    def get_histogram_images(self):       
        """
        Get histograms as JSON documents.
        @returns list of 1 JSON document containing the images.
        """
        image_list = []
        tag = "KL"
        keywords = ["KL"]
        description = "KL"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_kl)
        image_list.append(doc)
 
        return image_list

    def _cleanup_at_death(self):
        """
        Reinitialise histograms at death and print out new (empty) images
        """
        self.__init_histos()
        self.get_histogram_images()
