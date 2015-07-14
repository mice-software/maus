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
# pylint: disable = E1101
# Disable messages about too many branches and too many lines.
# pylint: disable = R0912
# pylint: disable = R0915
# pylint: disable = C0103, C0301
# pylint: disable = W0105, W0612, W0201

import ROOT
import maus_cpp.converter
from ReducePyROOTHistogram import ReducePyROOTHistogram

class ReducePySciFiPlot(ReducePyROOTHistogram): # pylint: disable=R0902
    """
    ReducePySciFiPlot plots several Tracker histograms.
    Currently the following histograms are filled:
    Digits per tracker and station,
    pe per channel and
    Spacepoints per station

    Histograms are drawn on different canvases.
    The canvases are refreshed every N spills where N = refresh_rate
    set via refresh_rate data card value (see below).

    The default is to run ROOT in batch mode
    To run in interactive mode, set root_batch_mode = 0 in the data card
    (see below).

    At the end of the run, the canvases are printed to eps files

    A sequence of 3 histograms are output as JSON documents of form:

    @verbatim
    {"image": {"keywords": [...list of image keywords...],
               "description":"...a description of the image...",
               "tag": TAG,
               "image_type": "eps", 
               "data": "...base 64 encoded image..."}}
    @endverbatim

    SciFi, Digits, PE Channel, Spacepoints

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

        #2D histos
        self.SciFiSpacepointsTwoDH = {}
        #Digit histos
        self.SciFiDigitT1 = None
        self.SciFiDigitT2 = None
        #PEperChannel histos
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
        #SP histos
        self.SciFiSpacepointsT1 = None
        self.SciFiSpacepointsT2 = None

        #Digits per channel Histos
        self.SciFiDigitsPerChannelT1S1P1 = None
        self.SciFiDigitsPerChannelT1S1P2 = None
        self.SciFiDigitsPerChannelT1S1P3 = None
        self.SciFiDigitsPerChannelT1S2P1 = None
        self.SciFiDigitsPerChannelT1S2P2 = None
        self.SciFiDigitsPerChannelT1S2P3 = None
        self.SciFiDigitsPerChannelT1S3P1 = None
        self.SciFiDigitsPerChannelT1S3P2 = None
        self.SciFiDigitsPerChannelT1S3P3 = None
        self.SciFiDigitsPerChannelT1S4P1 = None
        self.SciFiDigitsPerChannelT1S4P2 = None
        self.SciFiDigitsPerChannelT1S4P3 = None
        self.SciFiDigitsPerChannelT1S5P1 = None
        self.SciFiDigitsPerChannelT1S5P2 = None
        self.SciFiDigitsPerChannelT1S5P3 = None
        self.SciFiDigitsPerChannelT2S1P1 = None
        self.SciFiDigitsPerChannelT2S1P2 = None
        self.SciFiDigitsPerChannelT2S1P3 = None
        self.SciFiDigitsPerChannelT2S2P1 = None
        self.SciFiDigitsPerChannelT2S2P2 = None
        self.SciFiDigitsPerChannelT2S2P3 = None
        self.SciFiDigitsPerChannelT2S3P1 = None
        self.SciFiDigitsPerChannelT2S3P2 = None
        self.SciFiDigitsPerChannelT2S3P3 = None
        self.SciFiDigitsPerChannelT2S4P1 = None
        self.SciFiDigitsPerChannelT2S4P2 = None
        self.SciFiDigitsPerChannelT2S4P3 = None
        self.SciFiDigitsPerChannelT2S5P1 = None
        self.SciFiDigitsPerChannelT2S5P2 = None
        self.SciFiDigitsPerChannelT2S5P3 = None

        self.SciFiSpacepointsTwoDC = {}
        self.canvas_SciFiDigit = None
        self.canvas_SciFiPEperChannelT1 = None
        self.canvas_SciFiPEperChannelT2 = None
        self.canvas_SciFiSpacepoints = None
        self.canvas_SciFiDigitsPerChannelT1 = None
        self.canvas_SciFiDigitsPerChannelT2 = None
        #self.canvas_SciFiKunoPlot = None

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

        # Get SciFi digits & fill the relevant histograms.
        if data_spill and not self.get_SciFiDigits(spill): 
            raise ValueError("SciFi digits not in spill")

        # Get SciFi spacepoints & fill the relevant histograms.
        if data_spill and not self.get_SciFiSpacepoints(spill): 
            raise ValueError("SciFi spacepoints not in spill")

        if data_spill and not self.get_SciFiSpacepointsTwoD(spill): 
            raise ValueError("SciFi spacepoints not in spill")


        # Refresh canvases at requested frequency.
        #print self.refresh_rate
        if self.spill_count % self.refresh_rate == 0:
            self.update_histos()
            return self.get_histogram_images()
        else:
            return []

    def get_SciFiDigits(self, spill_data):

        """
        Get the SciFiDigits and update the histograms.
        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "digits" in
        the spill.
        """

        spill = spill_data.GetSpill()
        if spill.GetReconEventSize() == 0:
            raise ValueError("recon_events not in spill")
        reconevents = spill.GetReconEvents()
        # print '# recon events = ',reconevents[0].GetPartEventNumber()
        for evn in range(spill.GetReconEventSize()):
            sci_fi_event = reconevents[evn].GetSciFiEvent()
            if sci_fi_event is None:
                raise ValueError("sci_fi_event not in recon_events")
            # Return if we cannot find sci fi digits
            SciFiDigits = sci_fi_event.digits()
            if SciFiDigits is None:
                return False

            """
            If the tracker number = 0 fill ScifiDigitT1 with station Number
            and likewise for DS Tracker.
            """
            # Gives information on cabling efficiency
            title = "Run "+str(spill.GetRunNumber())+" Spill "+str(spill.GetSpillNumber())
            for i in range(SciFiDigits.size()):
                # print SciFiDigits[i].get_tracker()
                if (SciFiDigits[i].get_tracker() == 0):
                    self.SciFiDigitT1.Fill(SciFiDigits[i].get_station())
                    self.SciFiDigitT1.SetTitle("Spacepoints In US Tracker "+title+" (stn 1 is innermost)")
                if (SciFiDigits[i].get_tracker() == 1):
                    self.SciFiDigitT2.Fill(SciFiDigits[i].get_station())
                    self.SciFiDigitT2.SetTitle("Spacepoints In DS Tracker "+title+" (stn 1 is innermost)")

            # Gives information on fibre performance
            for i in range(SciFiDigits.size()):
                # print SciFiDigits[i].get_tracker()
                if (SciFiDigits[i].get_tracker() == 0):               
                    if (SciFiDigits[i].get_station() == 1):
                        self.SciFiPEperChannelT1S1.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())
                        self.SciFiPEperChannelT1S1.SetTitle("npe per Channel In US Tracker, Stations 1-5 "+title+" (stn 1 is innermost)")
                    if (SciFiDigits[i].get_station() == 2):
                        self.SciFiPEperChannelT1S2.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())
                    if (SciFiDigits[i].get_station() == 3):
                        self.SciFiPEperChannelT1S3.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())
                    if (SciFiDigits[i].get_station() == 4):
                        self.SciFiPEperChannelT1S4.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())
                    if (SciFiDigits[i].get_station() == 5):
                        self.SciFiPEperChannelT1S5.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())
                if (SciFiDigits[i].get_tracker() == 1):
                    if (SciFiDigits[i].get_station() == 1):
                        self.SciFiPEperChannelT2S1.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())
                        self.SciFiPEperChannelT2S1.SetTitle("npe per Channel In DS Tracker, Stations 1-5 "+title+" (stn 1 is innermost)")
                    if (SciFiDigits[i].get_station() == 2):
                        self.SciFiPEperChannelT2S2.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())
                    if (SciFiDigits[i].get_station() == 3):
                        self.SciFiPEperChannelT2S3.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())
                    if (SciFiDigits[i].get_station() == 4):
                        self.SciFiPEperChannelT2S4.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())
                    if (SciFiDigits[i].get_station() == 5):
                        self.SciFiPEperChannelT2S5.Fill(SciFiDigits[i].get_channel(), \
                        SciFiDigits[i].get_npe())

            # Gives information on Mapping
            for i in range(SciFiDigits.size()):
                if (SciFiDigits[i].get_tracker() == 0):               
                    if (SciFiDigits[i].get_station() == 1):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT1S1P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT1S1P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT1S1P3.Fill(SciFiDigits[i].get_channel())
                    if (SciFiDigits[i].get_station() == 2):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT1S2P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT1S2P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT1S2P3.Fill(SciFiDigits[i].get_channel())
                    if (SciFiDigits[i].get_station() == 3):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT1S3P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT1S3P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT1S3P3.Fill(SciFiDigits[i].get_channel())
                    if (SciFiDigits[i].get_station() == 4):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT1S4P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT1S4P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT1S4P3.Fill(SciFiDigits[i].get_channel())
                    if (SciFiDigits[i].get_station() == 5):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT1S5P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT1S5P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT1S5P3.Fill(SciFiDigits[i].get_channel())
                if (SciFiDigits[i].get_tracker() == 1):               
                    # print SciFiDigits[i].digits()
                    # print SciFiDigits[i].get_channel()
                    if (SciFiDigits[i].get_station() == 1):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT2S1P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT2S1P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT2S1P3.Fill(SciFiDigits[i].get_channel())
                    if (SciFiDigits[i].get_station() == 2):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT2S2P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT2S2P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT2S2P3.Fill(SciFiDigits[i].get_channel())
                    if (SciFiDigits[i].get_station() == 3):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT2S3P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT2S3P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT2S3P3.Fill(SciFiDigits[i].get_channel())
                    if (SciFiDigits[i].get_station() == 4):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT2S4P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT2S4P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT2S4P3.Fill(SciFiDigits[i].get_channel())
                    if (SciFiDigits[i].get_station() == 5):
                        if (SciFiDigits[i].get_plane() == 0):
                            self.SciFiDigitsPerChannelT2S5P1.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 1):
                            self.SciFiDigitsPerChannelT2S5P2.Fill(SciFiDigits[i].get_channel())
                        if (SciFiDigits[i].get_plane() == 2):
                            self.SciFiDigitsPerChannelT2S5P3.Fill(SciFiDigits[i].get_channel())

        return True

    # Gives information on reconstruction efficiency
    def get_SciFiSpacepoints(self, spill_data):

        """
        Get the SciFiDigits and update the histograms.

        @param self Object reference.
        @param spill Current spill.
        @return True if no errors or False if no "digits" in
        the spill.
        """

        if spill_data.GetSpill().GetReconEventSize() == 0:
            raise ValueError("recon_events not in spill")
        reconevents = spill_data.GetSpill().GetReconEvents()
        # print '# recon events = ',reconevents[0].GetPartEventNumber()
        for evn in range(spill_data.GetSpill().GetReconEventSize()):
            sci_fi_event = reconevents[evn].GetSciFiEvent()
            if sci_fi_event is None:
                raise ValueError("sci_fi_event not in recon_events")
            # Return if we cannot find sci fi spacepionts
            SciFiSpacepoints = sci_fi_event.spacepoints()
            if SciFiSpacepoints is None:
                return False

            '''If the tracker number =0 fill SciFiSpacepointsT1 with station Number
            and likewise for DS Tracker.'''
            spill = spill_data.GetSpill()
            title = "Run "+str(spill.GetRunNumber())+" Spill "+str(spill.GetSpillNumber())
            for i in range(SciFiSpacepoints.size()):
                # print SciFiSpacepoints[i].get_tracker(), SciFiSpacepoints[i].get_station()
                if (SciFiSpacepoints[i].get_tracker() == 0):
                    self.SciFiSpacepointsT1.Fill(SciFiSpacepoints[i].get_station())
                    self.SciFiSpacepointsT1.SetTitle("Spacepoints In US Tracker "+title+" (station 1 is innermost)")
                if (SciFiSpacepoints[i].get_tracker() == 1):
                    self.SciFiSpacepointsT2.Fill(SciFiSpacepoints[i].get_station())
                    self.SciFiSpacepointsT2.SetTitle("Spacepoints In DS Tracker "+title+" (station 1 is innermost)")
        return True

    def two_d_hist_name(self, tracker, station): # pylint: disable = R0201
        """Define 2D histogram."""
        return "tracker_"+str(tracker)+"_station_"+str(station)

    def get_SciFiSpacepointsTwoD(self, spill_data):
        """Get the 2D spacepoints for each tracker and station and fill histos."""

        spill = spill_data.GetSpill()
        title = "Run "+str(spill.GetRunNumber())+" Spill "+str(spill.GetSpillNumber())
        for key in self.SciFiSpacepointsTwoDH:
            local_name = key.replace("_", " ")
            self.SciFiSpacepointsTwoDH[key].SetTitle(title+" "+local_name+" (station 1 is innermost)")
        for recon_event in spill.GetReconEvents():
            scifi_event = recon_event.GetSciFiEvent()
            for space_point in scifi_event.spacepoints():
                station = space_point.get_station()
                tracker = space_point.get_tracker()
                name = self.two_d_hist_name(tracker, station)
                pos = space_point.get_position()
                self.SciFiSpacepointsTwoDH[name].Fill(pos.x(), pos.y())
                self.SciFiSpacepointsTwoDH[name].Fill(pos.x(), pos.y())

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

        #Digits
        self.SciFiDigitT1 = ROOT.TH1F("h1", "SciFi Digits in US Tracker", 100, 0, 6)
        self.SciFiDigitT1.GetXaxis().SetTitle("Stations")
        self.SciFiDigitT1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitT2 = ROOT.TH1F("h1", "SciFi Digits in DS Tracker", 100, 0, 6)
        self.SciFiDigitT2.GetXaxis().SetTitle("Stations")
        self.SciFiDigitT2.GetYaxis().SetTitle("Number of digits")

        #PEperChannel 
        self.SciFiPEperChannelT1S1 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 1 US Tracker", 215, 0, 215)
        self.SciFiPEperChannelT1S1.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT1S1.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT1S2 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 2 US Tracker", 215, 0, 215)
        self.SciFiPEperChannelT1S2.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT1S2.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT1S3 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 3 US Tracker", 215, 0, 215)
        self.SciFiPEperChannelT1S3.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT1S3.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT1S4 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 4 US Tracker", 215, 0, 215)
        self.SciFiPEperChannelT1S4.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT1S4.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT1S5 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 5 US Tracker", 215, 0, 215)
        self.SciFiPEperChannelT1S5.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT1S5.GetYaxis().SetTitle("Number of pe")

        self.SciFiPEperChannelT2S1 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 1 DS Tracker", 215, 0, 215)
        self.SciFiPEperChannelT2S1.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S1.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT2S2 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 2 DS Tracker", 215, 0, 215)
        self.SciFiPEperChannelT2S2.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S2.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT2S3 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 3 DS Tracker", 215, 0, 215)
        self.SciFiPEperChannelT2S3.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S3.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT2S4 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 4 DS Tracker", 215, 0, 215)
        self.SciFiPEperChannelT2S4.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S4.GetYaxis().SetTitle("Number of pe")
        self.SciFiPEperChannelT2S5 = ROOT.TH1F("h1", "SciFi Digits: pe per channel in Station 5 DS Tracker", 215, 0, 215)
        self.SciFiPEperChannelT2S5.GetXaxis().SetTitle("Channel number")
        self.SciFiPEperChannelT2S5.GetYaxis().SetTitle("Number of pe")

        #DigitsperChannel histos
        self.SciFiDigitsPerChannelT1S1P1 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 1, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S1P1.SetTitle("SciFi Digits per channel in US Tracker, Station 1, Plane 1")
        self.SciFiDigitsPerChannelT1S1P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S1P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S1P2 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 1, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S1P2.SetTitle("SciFi Digits per channel in US Tracker, Station 1, Plane 2")
        self.SciFiDigitsPerChannelT1S1P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S1P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S1P3 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 1, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S1P3.SetTitle("SciFi Digits per channel in US Tracker, Station 1, Plane 3")
        self.SciFiDigitsPerChannelT1S1P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S1P3.GetYaxis().SetTitle("Number of digits")

        self.SciFiDigitsPerChannelT1S2P1 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 2, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S2P1.SetTitle("SciFi Digits per channel in US Tracker, Station 2, Plane 1")
        self.SciFiDigitsPerChannelT1S2P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S2P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S2P2 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 2, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S2P2.SetTitle("SciFi Digits per channel in US Tracker, Station 2, Plane 2")
        self.SciFiDigitsPerChannelT1S2P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S2P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S2P3 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 2, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S2P3.SetTitle("SciFi Digits per channel in US Tracker, Station 2, Plane 3")
        self.SciFiDigitsPerChannelT1S2P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S2P3.GetYaxis().SetTitle("Number of digits")

        self.SciFiDigitsPerChannelT1S3P1 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 3, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S3P1.SetTitle("SciFi Digits per channel in US Tracker, Station 3, Plane 1")
        self.SciFiDigitsPerChannelT1S3P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S3P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S3P2 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 3, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S3P2.SetTitle("SciFi Digits per channel in US Tracker, Station 3, Plane 2")
        self.SciFiDigitsPerChannelT1S3P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S3P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S3P3 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 3, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S3P3.SetTitle("SciFi Digits per channel in US Tracker, Station 3, Plane 3")
        self.SciFiDigitsPerChannelT1S3P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S3P3.GetYaxis().SetTitle("Number of digits")

        self.SciFiDigitsPerChannelT1S4P1 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 4, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S4P1.SetTitle("SciFi Digits per channel in US Tracker, Station 4, Plane 1")
        self.SciFiDigitsPerChannelT1S4P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S4P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S4P2 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 4, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S4P2.SetTitle("SciFi Digits per channel in US Tracker, Station 4, Plane 2")
        self.SciFiDigitsPerChannelT1S4P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S4P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S4P3 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 4, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S4P3.SetTitle("SciFi Digits per channel in US Tracker, Station 4, Plane 3")
        self.SciFiDigitsPerChannelT1S4P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S4P3.GetYaxis().SetTitle("Number of digits")

        self.SciFiDigitsPerChannelT1S5P1 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 5, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S5P1.SetTitle("SciFi Digits per channel in US Tracker, Station 5, Plane 1")
        self.SciFiDigitsPerChannelT1S5P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S5P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S5P2 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 5, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S5P2.SetTitle("SciFi Digits per channel in US Tracker, Station 5, Plane 2")
        self.SciFiDigitsPerChannelT1S5P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S5P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT1S5P3 = ROOT.TH1F("h1", "SciFi Digits per channel in US Tracker, Station 5, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT1S5P3.SetTitle("SciFi Digits per channel in US Tracker, Station 5, Plane 3")
        self.SciFiDigitsPerChannelT1S5P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT1S5P3.GetYaxis().SetTitle("Number of digits")

        self.SciFiDigitsPerChannelT2S1P1 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 1, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S1P1.SetTitle("SciFi Digits per channel in DS Tracker, Station 1, Plane 1")
        self.SciFiDigitsPerChannelT2S1P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S1P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S1P2 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 1, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S1P2.SetTitle("SciFi Digits per channel in DS Tracker, Station 1, Plane 2")
        self.SciFiDigitsPerChannelT2S1P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S1P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S1P3 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 1, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S1P3.SetTitle("SciFi Digits per channel in DS Tracker, Station 1, Plane 3")
        self.SciFiDigitsPerChannelT2S1P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S1P3.GetYaxis().SetTitle("Number of digits")

        self.SciFiDigitsPerChannelT2S2P1 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 2, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S2P1.SetTitle("SciFi Digits per channel in DS Tracker, Station 2, Plane 1")
        self.SciFiDigitsPerChannelT2S2P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S2P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S2P2 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 2, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S2P2.SetTitle("SciFi Digits per channel in DS Tracker, Station 2, Plane 2")
        self.SciFiDigitsPerChannelT2S2P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S2P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S2P3 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 2, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S2P3.SetTitle("SciFi Digits per channel in DS Tracker, Station 2, Plane 3")
        self.SciFiDigitsPerChannelT2S2P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S2P3.GetYaxis().SetTitle("Number of digits")

        self.SciFiDigitsPerChannelT2S3P1 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 3, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S3P1.SetTitle("SciFi Digits per channel in DS Tracker, Station 3, Plane 1")
        self.SciFiDigitsPerChannelT2S3P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S3P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S3P2 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 3, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S3P2.SetTitle("SciFi Digits per channel in DS Tracker, Station 3, Plane 2")
        self.SciFiDigitsPerChannelT2S3P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S3P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S3P3 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 3, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S3P3.SetTitle("SciFi Digits per channel in DS Tracker, Station 3, Plane 3")
        self.SciFiDigitsPerChannelT2S3P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S3P3.GetYaxis().SetTitle("Number of digits")

        self.SciFiDigitsPerChannelT2S4P1 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 4, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S4P1.SetTitle("SciFi Digits per channel in DS Tracker, Station 4, Plane 1")
        self.SciFiDigitsPerChannelT2S4P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S4P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S4P2 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 4, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S4P2.SetTitle("SciFi Digits per channel in DS Tracker, Station 4, Plane 2")
        self.SciFiDigitsPerChannelT2S4P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S4P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S4P3 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 4, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S4P3.SetTitle("SciFi Digits per channel in DS Tracker, Station 4, Plane 3")
        self.SciFiDigitsPerChannelT2S4P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S4P3.GetYaxis().SetTitle("Number of digits")

        self.SciFiDigitsPerChannelT2S5P1 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 5, Plane 1", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S5P1.SetTitle("SciFi Digits per channel in DS Tracker, Station 5, Plane 1")
        self.SciFiDigitsPerChannelT2S5P1.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S5P1.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S5P2 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 5, Plane 2", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S5P2.SetTitle("SciFi Digits per channel in DS Tracker, Station 5, Plane 2")
        self.SciFiDigitsPerChannelT2S5P2.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S5P2.GetYaxis().SetTitle("Number of digits")
        self.SciFiDigitsPerChannelT2S5P3 = ROOT.TH1F("h1", "SciFi Digits per channel in DS Tracker, Station 5, Plane 3", 215, 0, 215)
        self.SciFiDigitsPerChannelT2S5P3.SetTitle("SciFi Digits per channel in DS Tracker, Station 5, Plane 3")
        self.SciFiDigitsPerChannelT2S5P3.GetXaxis().SetTitle("Channel number")
        self.SciFiDigitsPerChannelT2S5P3.GetYaxis().SetTitle("Number of digits")

        #Spacepoints histos
        self.SciFiSpacepointsT1 = ROOT.TH1F("h1", "SciFi Spacepoints in US Tracker", 100, 0, 6)
        self.SciFiSpacepointsT1.GetXaxis().SetTitle("Stations")
        self.SciFiSpacepointsT1.GetYaxis().SetTitle("Number of spacepoints")
        self.SciFiSpacepointsT2 = ROOT.TH1F("h1", "SciFi Spacepoints in DS Tracker", 100, 0, 6)
        self.SciFiSpacepointsT2.GetXaxis().SetTitle("Stations")
        self.SciFiSpacepointsT2.GetYaxis().SetTitle("Number of spacepoints")

        # 2D SP histos
        for tracker in range(2):
            for station in range(1, 6):
                name = self.two_d_hist_name(tracker, station)
                self.SciFiSpacepointsTwoDC[name] = ROOT.TCanvas(name, name)
                self.SciFiSpacepointsTwoDH[name] = ROOT.TH2D(name, name+";x [mm];y[ mm]", 100, -200., 200., 100, -200., 200.)
                self.SciFiSpacepointsTwoDH[name].Draw("COLZ")

        # Create canvases
        # Draw() of histos has to be done only once
        # for updating the histograms, just Modified() and Update() on canvases
        # the update/refresh is done in update_histos()

        self.canvas_SciFiDigit = ROOT.TCanvas("SciFiDigit", "SciFiDigit")
        #self.canvas_SciFiDigit.SetTitle("x")
        self.canvas_SciFiDigit.Divide(2, 1)
        self.canvas_SciFiDigit.cd(1)
        self.SciFiDigitT1.Draw()
        self.canvas_SciFiDigit.cd(2)
        self.SciFiDigitT2.Draw()

        self.canvas_SciFiPEperChannelT1 = ROOT.TCanvas("SciFiPEperChannelUS", "SciFiPEperChannelUS")
        self.canvas_SciFiPEperChannelT1.Divide(5, 1)
        self.canvas_SciFiPEperChannelT1.cd(1)
        self.SciFiPEperChannelT1S1.Draw()
        self.canvas_SciFiPEperChannelT1.cd(2)
        self.SciFiPEperChannelT1S2.Draw()
        self.canvas_SciFiPEperChannelT1.cd(3)
        self.SciFiPEperChannelT1S3.Draw()
        self.canvas_SciFiPEperChannelT1.cd(4)
        self.SciFiPEperChannelT1S4.Draw()
        self.canvas_SciFiPEperChannelT1.cd(5)
        self.SciFiPEperChannelT1S5.Draw()
        self.canvas_SciFiPEperChannelT2 = ROOT.TCanvas("SciFiPEperChannelDS", "SciFiPEperChannelDS")
        self.canvas_SciFiPEperChannelT2.Divide(5, 1)
        self.canvas_SciFiPEperChannelT2.cd(1)
        self.SciFiPEperChannelT2S1.Draw()
        self.canvas_SciFiPEperChannelT2.cd(2)
        self.SciFiPEperChannelT2S2.Draw()
        self.canvas_SciFiPEperChannelT2.cd(3)
        self.SciFiPEperChannelT2S3.Draw()
        self.canvas_SciFiPEperChannelT2.cd(4)
        self.SciFiPEperChannelT2S4.Draw()
        self.canvas_SciFiPEperChannelT2.cd(5)
        self.SciFiPEperChannelT2S5.Draw()


        self.canvas_SciFiDigitsPerChannelT1 = ROOT.TCanvas("SciFiDigitsPerChannelUS", "SciFiDigitsPerChannelUS")
        self.canvas_SciFiDigitsPerChannelT1.Divide(5, 3)
        self.canvas_SciFiDigitsPerChannelT1.cd(1)
        self.SciFiDigitsPerChannelT1S1P1.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(2)
        self.SciFiDigitsPerChannelT1S2P1.Draw()       
        self.canvas_SciFiDigitsPerChannelT1.cd(3)
        self.SciFiDigitsPerChannelT1S3P1.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(4)
        self.SciFiDigitsPerChannelT1S4P1.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(5)
        self.SciFiDigitsPerChannelT1S5P1.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(6)
        self.SciFiDigitsPerChannelT1S1P2.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(7)
        self.SciFiDigitsPerChannelT1S2P2.Draw()       
        self.canvas_SciFiDigitsPerChannelT1.cd(8)
        self.SciFiDigitsPerChannelT1S3P2.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(9)
        self.SciFiDigitsPerChannelT1S4P2.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(10)
        self.SciFiDigitsPerChannelT1S5P2.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(11)
        self.SciFiDigitsPerChannelT1S1P3.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(12)
        self.SciFiDigitsPerChannelT1S2P3.Draw()       
        self.canvas_SciFiDigitsPerChannelT1.cd(13)
        self.SciFiDigitsPerChannelT1S3P3.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(14)
        self.SciFiDigitsPerChannelT1S4P3.Draw()
        self.canvas_SciFiDigitsPerChannelT1.cd(15)
        self.SciFiDigitsPerChannelT1S5P3.Draw()

        self.canvas_SciFiDigitsPerChannelT2 = ROOT.TCanvas("SciFiDigitsPerChannelDS", "SciFiDigitsPerChannelDS")
        self.canvas_SciFiDigitsPerChannelT2.Divide(5, 3)
        self.canvas_SciFiDigitsPerChannelT2.cd(1)
        self.SciFiDigitsPerChannelT2S1P1.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(2)
        self.SciFiDigitsPerChannelT2S2P1.Draw()       
        self.canvas_SciFiDigitsPerChannelT2.cd(3)
        self.SciFiDigitsPerChannelT2S3P1.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(4)
        self.SciFiDigitsPerChannelT2S4P1.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(5)
        self.SciFiDigitsPerChannelT2S5P1.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(6)
        self.SciFiDigitsPerChannelT2S1P2.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(7)
        self.SciFiDigitsPerChannelT2S2P2.Draw()       
        self.canvas_SciFiDigitsPerChannelT2.cd(8)
        self.SciFiDigitsPerChannelT2S3P2.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(9)
        self.SciFiDigitsPerChannelT2S4P2.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(10)
        self.SciFiDigitsPerChannelT2S5P2.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(11)
        self.SciFiDigitsPerChannelT2S1P3.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(12)
        self.SciFiDigitsPerChannelT2S2P3.Draw()       
        self.canvas_SciFiDigitsPerChannelT2.cd(13)
        self.SciFiDigitsPerChannelT2S3P3.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(14)
        self.SciFiDigitsPerChannelT2S4P3.Draw()
        self.canvas_SciFiDigitsPerChannelT2.cd(15)
        self.SciFiDigitsPerChannelT2S5P3.Draw()

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
        self.canvas_SciFiSpacepoints.Update()
        self.canvas_SciFiPEperChannelT1.Update()
        self.canvas_SciFiPEperChannelT2.Update()
        self.canvas_SciFiDigitsPerChannelT1.Update()
        self.canvas_SciFiDigitsPerChannelT2.Update()
        for key in self.SciFiSpacepointsTwoDC:
            self.SciFiSpacepointsTwoDC[key].Update()

    def get_histogram_images(self):
        """
        Get histograms as JSON documents.
        @returns list of 1 JSON document containing the images.
        """

        image_list = []

        #digits per station
        tag = "SciFi_Digits"
        keywords = ["SciFi", "Digits"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiDigit)
        image_list.append(doc)

        #pe per channel
        tag = "SciFi_PEperChannelT1"
        keywords = ["SciFi", "PE", "Channel", "US"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiPEperChannelT1)
        image_list.append(doc)

        #pe per channel
        tag = "SciFi_PEperChannelT2"
        keywords = ["SciFi", "PE", "Channel", "DS"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiPEperChannelT2)
        image_list.append(doc)

        #Digits per channel 
        tag = "SciFi_DigitsPerChannelT1"
        keywords = ["SciFi", "Digits", "Plane", "Channel", "US"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiDigitsPerChannelT1)
        image_list.append(doc)

        #Digits per channel 
        tag = "SciFi_DigitsPerChannelT2"
        keywords = ["SciFi", "Digits", "Plane", "Channel", "DS"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiDigitsPerChannelT2)
        image_list.append(doc)

        # Spacepoints per station
        tag = "SciFi_Spacepoints"
        keywords = ["SciFi", "Spacepoints"]
        description = "SciFi"
        doc = ReducePyROOTHistogram.get_image_doc( \
            self, keywords, description, tag, self.canvas_SciFiSpacepoints)
        image_list.append(doc)

        # 2D Spacepoints per station
        tag_pref = "SciFi_SpacePointsTwoD_"
        keywords = ["SciFi", "Spacepoints", "2D"]
        description = "SciFi"
        for key in self.SciFiSpacepointsTwoDC:
            doc = ReducePyROOTHistogram.get_image_doc( \
                self, keywords, description, tag_pref+key, self.SciFiSpacepointsTwoDC[key])
            image_list.append(doc)

        return image_list

    def _cleanup_at_death(self):
        """
        Reinitialise histograms at death and print out new (empty) images
        """
        self.__init_histos()
        self.get_histogram_images()

