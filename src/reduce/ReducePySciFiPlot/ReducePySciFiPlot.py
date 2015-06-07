"""
ReducePySciFiPlot fills Tracker histograms for online reconstruction.  It produces histograms of Hits per plane, Spacepoints per station and event displays.
It draws them, refreshes the canvases and prints to eps at the end of
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

class ReducePySciFiPlot(ReducePyROOTHistogram): # pylint: disable=R0902
    """gedit 
    ReducePySciFiPlot plots several Tracker histograms.
    Currently the following histograms are filled:
    - XXXXXX  DESCRIBE PLOTS HERE
    
    Histograms are drawn on different canvases.
    The canvases are refreshed every N spills where N = refresh_rate
    set via refresh_rate data card value (see below).
   
    The default is to run ROOT in batch mode
    To run in interactive mode, set root_batch_mode = 0 in the data card
    (see below). 
    
    At the end of the run, the canvases are printed to eps files

    A sequence of XXX histogram is output as JSON documents of form:

    @verbatim
    {"image": {"keywords": [...list of image keywords...],
               "description":"...a description of the image...",
               "tag": TAG,
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}}
    @endverbatim

	DESCRIBE NAMES of TAGS HERE!!!!

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

	self.SciFiDigitT1 = None
	self.SciFiDigitT2 = None
	self.SciFiPEperChannelT1S1 = None
	self.SciFiPEperChannelT1S2 = None
	self.SciFiPEperChannelT1S3 = None
	self.SciFiPEperChannelT1S4 = None
	self.SciFiPEperChannelT1S5 = None
	self.SciFiPEperChannelT2S1 = None
	self.SciFiPEperChannelT2S2 = None
	self.SciFiPEperChannelT2S3 = None
	self.SciFiPEperChannelT2S4 = None
	self.SciFiPEperChannelT2S5 = None
	self.SciFiSpacepointsT1 = None
	self.SciFiSpacepointsT2 = None

	self.canvas_SciFiDigit = None	
 	self.canvas_SciFiPEperChannel = None      
	self.canvas_SciFiSpacepoints = None

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

        # Get SciFi digits & fill the relevant histograms.
        if not self.get_SciFiDigits(spill): 
            raise ValueError("SciFi digits not in spill")

        # Get SciFi spacepoints & fill the relevant histograms.
        if not self.get_SciFiSpacepoints(spill): 
            raise ValueError("SciFi spacepoints not in spill")

        # Refresh canvases at requested frequency.
        #print self.refresh_rate
        if self.spill_count % self.refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return []

    def get_SciFiDigits(self, spill):
        
	""" 
        Get the SciFiDigits and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "digits" in
        the spill.
        """
        if 'recon_events' not in spill:
            raise ValueError("recon_events not in spill")
        # print 'nevt = ', len(spill['recon_events']), spill['recon_events']
        for event in range(len(spill['recon_events'])):
            if 'sci_fi_event' not in spill['recon_events'][event]:
                #print 'no scifi event'
                return False
            print 
                # Return if we cannot find sci fi digits in the spill.
            if 'digits' not in \
                spill['recon_events'][event]['sci_fi_event']:
                return False

            '''
            If the tracker number = 0 fill SciFiDigitT1 with station Number
            and likewise for tracker 2.
            '''
            # Gives information on cabling efficiency
            SciFiDigits = spill['recon_events'][event]['sci_fi_event']['digits']
            for i in range(len(SciFiDigits)):
                #print SciFiDigits[i]['tracker']
                if (SciFiDigits[i]['tracker'] == 0):
                    self.SciFiDigitT1.Fill(SciFiDigits[i]['station'])
                if (SciFiDigits[i]['tracker'] == 1):
                    self.SciFiDigitT2.Fill(SciFiDigits[i]['station'])

            # Gives information on fibre performance
            for i in range(len(SciFiDigits)):
                #print SciFiDigits[i]['tracker']
                if (SciFiDigits[i]['tracker'] == 0):               
                    #print SciFiDigits[i]['npe']
                    #print SciFiDigits[i]['channel']
                    if (SciFiDigits[i]['station'] == 1):
                        self.SciFiPEperChannelT1S1.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])
                    if (SciFiDigits[i]['station'] == 2):
                        self.SciFiPEperChannelT1S2.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])
                    if (SciFiDigits[i]['station'] == 3):
                        self.SciFiPEperChannelT1S3.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])
                    if (SciFiDigits[i]['station'] == 4):
                        self.SciFiPEperChannelT1S4.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])
                    if (SciFiDigits[i]['station'] == 5):
                        self.SciFiPEperChannelT1S5.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])
                if (SciFiDigits[i]['tracker'] == 1):
                    if (SciFiDigits[i]['station'] == 1):
                        self.SciFiPEperChannelT2S1.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])
                    if (SciFiDigits[i]['station'] == 2):
                        self.SciFiPEperChannelT2S2.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])
                    if (SciFiDigits[i]['station'] == 3):
                        self.SciFiPEperChannelT2S3.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])
                    if (SciFiDigits[i]['station'] == 4):
                        self.SciFiPEperChannelT2S4.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])
                    if (SciFiDigits[i]['station'] == 5):
                        self.SciFiPEperChannelT2S5.Fill(SciFiDigits[i]['channel'], SciFiDigits[i]['npe'])

        return True

    # Gives information on reconstruction efficiency
    def get_SciFiSpacepoints(self, spill):
        
	""" 
        Get the SciFiDigits and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "digits" in
        the spill.
        """
        if 'recon_events' not in spill:
            raise ValueError("recon_events not in spill")
        # print 'nevt = ', len(spill['recon_events']), spill['recon_events']
        for event in range(len(spill['recon_events'])):
            if 'sci_fi_event' not in spill['recon_events'][event]:
                #print 'no scifi event'
                return False
            print 
                # Return if we cannot find sci fi digits in the spill.
            if 'spacepoints' not in \
                spill['recon_events'][event]['sci_fi_event']:
                return False

            '''
            If the tracker number =0 fill SciFiSpacepointsT1 with station Number
            and likewise for tracker 2.
            '''
            SciFiSpacepoints = spill['recon_events'][event]['sci_fi_event']['spacepoints']
            for i in range(len(SciFiSpacepoints)):
                print SciFiSpacepoints[i]['tracker']
                if (SciFiSpacepoints[i]['tracker'] == 0):
                    self.SciFiSpacepointsT1.Fill(SciFiSpacepoints[i]['station'])
                if (SciFiSpacepoints[i]['tracker'] == 1):
                    self.SciFiSpacepointsT2.Fill(SciFiSpacepoints[i]['station'])

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
 
        # define histograms
        self.SciFiDigitT1 = ROOT.TH1F("h1", "SciFi Digits in Tracker 1", 100, 0, 6)
        self.SciFiDigitT1.SetTitle("SciFi Digits Tracker 1")
        self.SciFiDigitT1.GetXaxis().SetTitle("Stations")
        self.SciFiDigitT1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitT2 = ROOT.TH1F("h1", "SciFi Digits in Tracker 2", 100, 0, 6)
        self.SciFiDigitT2.SetTitle("SciFi Digits Tracker 2")
        self.SciFiDigitT2.GetXaxis().SetTitle("Stations")
        self.SciFiDigitT2.GetYaxis().SetTitle("Number of digits")

        self.SciFiPEperChannelT1S1 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 1 Tracker 1", 215, 0, 215)
        self.SciFiPEperChannelT1S1.SetTitle("SciFi Digits: pe per channel in Station 1 in Tracker 1")
        self.SciFiPEperChannelT1S1.GetXaxis().SetTitle("Channel number")
        #self.SciFiPEperChannelT1S1.GetYaxis().SetTitleOffset(0.0)
        self.SciFiPEperChannelT1S1.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT1S2 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 2 Tracker 1", 215, 0, 215)
        self.SciFiPEperChannelT1S2.SetTitle("SciFi Digits: pe per channel in Station 2 in Tracker 1")
        self.SciFiPEperChannelT1S2.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT1S2.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT1S3 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 3 Tracker 1", 215, 0, 215)
        self.SciFiPEperChannelT1S3.SetTitle("SciFi Digits: pe per channel in Station 3 in Tracker 1")
        self.SciFiPEperChannelT1S3.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT1S3.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT1S4 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 4 Tracker 1", 215, 0, 215)
        self.SciFiPEperChannelT1S4.SetTitle("SciFi Digits: pe per channel in Station 4 in Tracker 1")
        self.SciFiPEperChannelT1S4.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT1S4.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT1S5 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 5 Tracker 1", 215, 0, 215)
        self.SciFiPEperChannelT1S5.SetTitle("SciFi Digits: pe per channel in Station 5 in Tracker 1")
        self.SciFiPEperChannelT1S5.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT1S5.GetYaxis().SetTitle("Number of pe")

        self.SciFiPEperChannelT2S1 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 1 Tracker 2", 215, 0, 215)
        self.SciFiPEperChannelT2S1.SetTitle("SciFi Digits: pe per channel in Station 1 in Tracker 2")
        self.SciFiPEperChannelT2S1.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S1.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT2S2 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 2 Tracker 2", 215, 0, 215)
        self.SciFiPEperChannelT2S2.SetTitle("SciFi Digits: pe per channel in Station 2 in Tracker 2")
        self.SciFiPEperChannelT2S2.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S2.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT2S3 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 3 Tracker 2", 215, 0, 215)
        self.SciFiPEperChannelT2S3.SetTitle("SciFi Digits: pe per channel in Station 3 in Tracker 2")
        self.SciFiPEperChannelT2S3.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S3.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT2S4 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 4 Tracker 2", 215, 0, 215)
        self.SciFiPEperChannelT2S4.SetTitle("SciFi Digits: pe per channel in Station 4 in Tracker 2")
        self.SciFiPEperChannelT2S4.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S4.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT2S5 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 5 Tracker 2", 215, 0, 215)
        self.SciFiPEperChannelT2S5.SetTitle("SciFi Digits: pe per channel in Station 5 in Tracker 2")
        self.SciFiPEperChannelT2S5.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S5.GetYaxis().SetTitle("Number of pe")


        self.SciFiSpacepointsT1 = ROOT.TH1F("h1", "SciFi Spacepoints in Tracker 1", 100, 0, 6)
        self.SciFiSpacepointsT1.SetTitle("SciFi Spacepoints Tracker 1")
        self.SciFiSpacepointsT1.GetXaxis().SetTitle("Stations")
        self.SciFiSpacepointsT1.GetYaxis().SetTitle("Number of spacepoints")
        self.SciFiSpacepointsT2 = ROOT.TH1F("h1", "SciFi Spacepoints in Tracker 2", 100, 0, 6)
        self.SciFiSpacepointsT2.SetTitle("SciFi Spacepoints Tracker 2")
        self.SciFiSpacepointsT2.GetXaxis().SetTitle("Stations")
        self.SciFiSpacepointsT2.GetYaxis().SetTitle("Number of spacepoints")

        # Create canvases
        # Draw() of histos has to be done only once
        # for updating the histograms, just Modified() and Update() on canvases
        # the update/refresh is done in update_histos()
        
        self.canvas_SciFiDigit = ROOT.TCanvas("SciFiDigit", "SciFiDigit")
        self.canvas_SciFiDigit.Divide(2, 1)
        self.canvas_SciFiDigit.cd(1)
        self.SciFiDigitT1.Draw()
        self.canvas_SciFiDigit.cd(2)
        self.SciFiDigitT2.Draw()
         
        self.canvas_SciFiPEperChannel = ROOT.TCanvas("SciFiPEperChannel", "SciFiPEperChannel")
        self.canvas_SciFiPEperChannel.Divide(5, 2)
        self.canvas_SciFiPEperChannel.cd(1)
        self.SciFiPEperChannelT1S1.Draw()
        self.canvas_SciFiPEperChannel.cd(2)
        self.SciFiPEperChannelT1S2.Draw()       
        self.canvas_SciFiPEperChannel.cd(3)
        self.SciFiPEperChannelT1S3.Draw()
        self.canvas_SciFiPEperChannel.cd(4)
        self.SciFiPEperChannelT1S4.Draw()
        self.canvas_SciFiPEperChannel.cd(5)
        self.SciFiPEperChannelT1S5.Draw()
        self.canvas_SciFiPEperChannel.cd(6)
        self.SciFiPEperChannelT2S1.Draw()
        self.canvas_SciFiPEperChannel.cd(7)
        self.SciFiPEperChannelT2S2.Draw()       
        self.canvas_SciFiPEperChannel.cd(8)
        self.SciFiPEperChannelT2S3.Draw()
        self.canvas_SciFiPEperChannel.cd(9)
        self.SciFiPEperChannelT2S4.Draw()
        self.canvas_SciFiPEperChannel.cd(10)
        self.SciFiPEperChannelT2S5.Draw()

        self.canvas_SciFiSpacepoints = ROOT.TCanvas("SciFiSpacepoints", "SciFiSpacepoints")
        self.canvas_SciFiSpacepoints.Divide(2, 1)
        self.canvas_SciFiSpacepoints.cd(1)
        self.SciFiSpacepointsT1.Draw()
        self.canvas_SciFiSpacepoints.cd(2)
        self.SciFiSpacepointsT2.Draw() 
       
        return True

    def update_histos(self):       
        """
        Updates histogram canvases. This is called only when the
        number of spills is divisible by the refresh rate.
        @param self Object reference.
        """

        self.canvas_SciFiDigit.Update()
	self.canvas_SciFiPEperChannel.Update()
        self.canvas_SciFiSpacepoints.Update()

    def get_histogram_images(self):       
        """
        Get histograms as JSON documents.
        @returns list of 1 JSON document containing the images.
        """

        image_list = []

        # ROOT
        # file label = PTM1-8.eps
        histos = self.SciFiDigitT1, self.SciFiDigitT2, self.SciFiPEperChannelT1S1,	self.SciFiPEperChannelT1S2, self.SciFiPEperChannelT1S3,	self.SciFiPEperChannelT1S4,	self.SciFiPEperChannelT1S5, self.SciFiPEperChannelT2S1,	self.SciFiPEperChannelT2S2,	self.SciFiPEperChannelT2S3, self.SciFiPEperChannelT2S4, self.SciFiPEperChannelT2S5, self.SciFiSpacepointsT1, self.SciFiSpacepointsT2
        tag = __name__
        content = __name__
        doc = ReducePyROOTHistogram.get_root_doc(self, [], content, tag, histos)
        image_list.append(doc)

        #digits per station
        tag = "SciFi_Digits"
        keywords = ["SciFi", "Digits"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiDigit)
        image_list.append(doc)

        #pe per channel
        image_list = []
        tag = "SciFi_PEperChannel"
        keywords = ["SciFi", "PE", "Channel"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiPEperChannel)
        image_list.append(doc)

        # Spacepoints per station
        tag = "SciFi_Spacepoints"
        keywords = ["SciFi", "Spacepoints"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiSpacepoints)
        image_list.append(doc)
 
        return image_list

    def _cleanup_at_death(self):
        """
        Reinitialise histograms at death and print out new (empty) images
        """
        self.__init_histos()
        self.get_histogram_images()






